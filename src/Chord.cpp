/*
 * Chord
 * File:     Chord.cpp
 * Author:   Evan Wilde                    <etcwilde@uvic.ca>
 * Date:     Jun 02 2015
 */

#include "Chord.hpp"

using namespace DNS;

ChordDNS::ChordDNS(const std::string& uid):
	m_primary_socket(NULL),
	m_uid(uid),
	m_uid_hash(Hash(uid)),
	m_dead(false),
	m_chord_log("logs/"+std::to_string(getpid())+"_chord.log")
{
	m_successor.set = false;
	m_successor.heartbeat = CHORD_DEFAULT_HEART_BEAT;
	m_successor.resiliancy = CHORD_DEFAULT_RESILLIANCY;
	m_successor.missed = false;
	m_predecessor.set = false;
	m_heart = std::thread(&ChordDNS::heartBeat, this);
}

ChordDNS::ChordDNS::~ChordDNS()
{
	m_dead = true;
	m_heart.join();
	m_primary_socket->shutdown();
	for (unsigned int i = 0; i < CHORD_DEFAULT_HANDLER_THREADS; ++i)
		m_handler_threads[i].join();
	if (m_primary_socket) delete m_primary_socket;
}

int ChordDNS::Lookup(const std::string& Name,
		std::string& ip, unsigned short& port)
{
	if (m_uid.compare(Name) == 0)
	{
		ip = "localhost";
		port = m_port;
		return 0;
	}

	if (!m_successor.set) return 1;

	if (m_successor.uid_hash == Hash(Name))
	{
		ip = m_successor.ip;
		port = m_successor.port;
		return 0;
	}



	// Okay, if we have a successor, go out and send them a request
	// I'm not really sure how to uh, fix this.
	Request get_request;
	std::string get_message;
	get_request.set_id(Hash(Name).raw());
	std::cout << "Packing Search Request:\n" <<
		Hash(get_request.id(), true).toString() << '\n';
	get_request.set_forward(false);
	get_request.set_type(Request::GET);
	get_request.SerializeToString(&get_message);
	m_primary_socket->write(get_message, m_successor.ip, m_successor.port);

	return 1;
	// Now we need to figure out how to get the result from the response.
	//
	// I'm thinking an event queue that services the requests as they come
	// in. Kind of like the TCP acknowledgment window but with ip/port
	// pairs instead of data packets.
}

void ChordDNS::Dump(const std::string& dump_name)
{
	Log dump_log(dump_name + ".dump");
	dump_log.write("-----------------------------------------");
	dump_log.write(m_uid + "::" + m_uid_hash.toString() + '\n');
	dump_log.write("Successor:");
	if (m_successor.set)
	{
		dump_log.write(m_successor.uid_hash.toString());
		dump_log.write("IP: " + m_successor.ip);
		dump_log.write("Port: " + std::to_string(m_successor.port) + '\n');
	}
	else dump_log.write("No successor\n");
}


// Network stuff
void ChordDNS::create(unsigned short port)
{
	m_port = port;
	for (unsigned int i = 0; i < CHORD_DEFAULT_HANDLER_THREADS; ++i)
		m_handler_threads.push_back(
				std::thread(&ChordDNS::request_handler, this));
	m_primary_socket = new UDPSocket(port);
}

// Join methods

void ChordDNS::join(const std::string& host_ip,
		unsigned short host_port, unsigned short my_port)
{
	//std::cout << " Joining\n";
	create(my_port);
	// Now send join request
	Request join_request;
	std::string join_message;

	join_request.set_id(m_uid_hash.raw());
	join_request.set_port(my_port);
	join_request.set_type(Request::JOIN);

	join_request.SerializeToString(&join_message);
	m_primary_socket->write(join_message, host_ip, host_port);
}


// Request handling
void ChordDNS::request_handler()
{
	while (m_primary_socket == NULL) {} //Spin lock
	while (!m_dead)
	{
		Request current_request;
		std::string message;
		std::string client_ip;
		unsigned short client_port;
		m_primary_socket->read(message, client_ip, client_port);

		if (m_dead) break;
		current_request.ParseFromString(message);

		/*std::cout << " Parsed incoming Message\n\t"
			<< "Type: " << current_request.type() << "\n\t"
			<< "Ip: " << current_request.ip() << "\n\t"
			<< "port: " << current_request.port() << "\n\t"
			<< "Forwarded: " << current_request.forward() << "\n\t"
			<< "Sender IP: " << client_ip << ':' << client_port << '\n';
		*/

/*		m_chord_log.write("Request: "
				+ std::to_string(current_request.type())
				+ " FROM " + client_ip
				+ ":" + std::to_string(client_port)); */


		switch(current_request.type())
		{
			case Request::JOIN:
				{
					//std::cout << "JOIN REQUEST\n";
					handle_join(current_request, client_ip,
							client_port);
				}
				break;
			case Request::SYNC:
				{
					handle_sync(current_request, client_ip,
							client_port);
				}
				break;
			case Request::GET:
				{
					//std::cout << " Handle Get\n";
					handle_get(current_request, client_ip,
							client_port);
				}
				break;
			default:
				{
					std::cerr <<"Request Protocol Error\n";
					m_chord_log.write("Request Protocol Error: " + std::to_string(current_request.type()));
				}
				break;
		}
	}
}

void ChordDNS::handle_join(const Request& req, const std::string& ip,
		unsigned short port)
{
	Hash client_hash(req.id(), true);
	if (req.forward())
	{
		std::cout << "Join Message forwarded: " << req.ip()
			<< ":" << req.port() << " " << client_hash.toString()
			<< '\n';
	}
	else
	{
		std::cout << "Join Message from: " << ip << ":" << port
			<< ":" << req.port() << " " << client_hash.toString()
			<< '\n';
	}


	if (!m_successor.set)
	{
		std::cout << " Setting initial Successor\n";

		// Configure struct
		m_successor.set = true;
		m_successor.resiliancy = CHORD_DEFAULT_RESILLIANCY;
		m_successor.heartbeat = CHORD_DEFAULT_HEART_BEAT;
		m_successor.missed = false;

		// Client configurations
		m_successor.uid_hash = client_hash;
		if (req.port() != 0) m_successor.port = req.port();
		else m_successor.port = port;
		if (req.forward())
		{
			std::cerr << "WARNING: INITIAL JOIN REQUEST FORWARDED\n";
			m_successor.ip = req.ip();
		}
		else m_successor.ip = ip;

		// Complete Chord Ring
		Request join_request;
		std::string join_message;
		join_request.set_port(m_port);
		join_request.set_id(m_uid_hash.raw());
		join_request.set_type(Request::JOIN);

		join_request.SerializeToString(&join_message);
		m_primary_socket->write(join_message, m_successor.ip,
				m_successor.port);

	}
	else if (client_hash == m_successor.uid_hash)
	{
		if ((req.forward() && req.ip() == m_successor.ip &&
				req.port() == m_successor.port)
				||
				(!req.forward() && ip == m_successor.ip &&
				 req.port() == m_successor.port))
		{
			std::cout << "This is noise. Dissolved...\n";
		}
		else
		{
			std::cout << " duplicate domain name\n";
			// Send failed join response
			// Unless my successor has died, in which case,
			// m_successor would not be this and it would got to
			// the next case... so yeah, this will handle node
			// failures probably
		}
	}
	else if (client_hash.between(m_uid_hash, m_successor.uid_hash))
	{
		std::cout << " You go between us!\n";
		// Make my old successor your successor
		Request neighbor_join;
		std::string neighbor_message;
		neighbor_join.set_id(m_successor.uid_hash.raw());
		neighbor_join.set_ip(m_successor.ip);
		neighbor_join.set_port(m_successor.port);
		neighbor_join.set_forward(true);
		neighbor_join.set_type(Request::JOIN);
		neighbor_join.SerializeToString(&neighbor_message);
		if (req.forward())
			m_primary_socket->write(neighbor_message, req.ip(),
					req.port());
		else
			m_primary_socket->write(neighbor_message, ip,
					req.port());

		m_successor.missed = false;
		m_successor.resiliancy = CHORD_DEFAULT_RESILLIANCY;
		m_successor.heartbeat = CHORD_DEFAULT_HEART_BEAT;
		if (req.forward()) m_successor.ip = req.ip();
		else m_successor.ip = ip;
		m_successor.port = req.port();
		m_successor.uid_hash = client_hash;
	}
	else
	{
		std::cout << " You get forwarded\n";
		Request forwarded_join;
		std::string join_message;
		forwarded_join.set_id(client_hash.raw());
		if (req.forward()) forwarded_join.set_ip(req.ip());
		else forwarded_join.set_ip(ip);
		forwarded_join.set_port(req.port());
		forwarded_join.set_type(Request::JOIN);
		forwarded_join.set_forward(true);
		forwarded_join.SerializeToString(&join_message);
		m_primary_socket->write(join_message, m_successor.ip,
				m_successor.port);
	}
}

void ChordDNS::handle_get(const Request& req, const std::string& ip,
		unsigned short port)
{
	Hash search_hash(req.id(), true);
	std::cout << "\tHandle GET\n";
	std::cout << "Looking for: \n" << search_hash.toString() << '\n'
		<< m_uid_hash.toString() << '\n'
		<< m_successor.uid_hash.toString() << '\n';
	if (search_hash == m_successor.uid_hash)
	{
		if (req.forward())
		{
			std::cout << " GET from "
				<< req.ip() << ":" << req.port()
				<< "\tResolved to: " << m_successor.ip
				<< ":" << m_successor.port << '\n';
		}
		else
		{
			std::cout << " GET from "
				<< ip << ":" << req.port()
				<< "\tResolved to: " << m_successor.ip
				<< ":" << m_successor.port << '\n';
		}

	}
	else if (search_hash == m_uid_hash)
	{
		std::cout << " You found me\n";
	}
	else if (search_hash.between(m_uid_hash, m_successor.uid_hash))
	{
		std::cout << " Search Not found\n";
	}
	else
	{
		Request get_request;
		std::string get_message;
		get_request.set_id(req.id());
		get_request.set_type(Request::GET);
		if (req.forward()) get_request.set_ip(req.ip());
		else get_request.set_ip(ip);
		get_request.set_port(req.port());
		get_request.set_forward(true);
		get_request.SerializeToString(&get_message);
		m_primary_socket->write(get_message, m_successor.ip,
				m_successor.port);

	}
}

/**
 * This will always be directly from our predecessor or successor
 * We will use the forward flag as a way of determining if it is a response
 */
void ChordDNS::handle_sync(const Request& req, const std::string& ip,
		unsigned short port)
{
	// Handle Response
	if (req.forward() == true) m_successor.missed = false;
	else // Handle sync request
	{
		Request sync_request;
		std::string sync_message;
		sync_request.set_id(m_uid_hash.raw());
		sync_request.set_type(Request::SYNC);
		sync_request.set_port(m_port);
		sync_request.set_forward(true);
		sync_request.SerializeToString(&sync_message);
		m_primary_socket->write(sync_message, ip, port);
	}
}


// Heart Stuff
void ChordDNS::pulse()
{
	if (m_successor.set)
	{
		Request sync_request;
		std::string sync_message;
		sync_request.set_id(m_uid_hash.raw());
		sync_request.set_type(Request::SYNC);
		sync_request.set_port(m_port);
		sync_request.set_forward(false);

		sync_request.SerializeToString(&sync_message);
		m_primary_socket->write(sync_message, m_successor.ip,
				m_successor.port);

		if (m_successor.missed == false)
			m_successor.resiliancy = CHORD_DEFAULT_RESILLIANCY;
		else if (m_successor.resiliancy == 0)
		{
			std::cout << " Successor died\n";
			m_successor.set = false;
		}
		else m_successor.resiliancy--;
		m_successor.missed = true;
	}
}

void ChordDNS::heartBeat()
{
	while (!m_dead) { pulse(); usleep(m_successor.heartbeat * 1000); }
}
