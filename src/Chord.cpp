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
	m_dead(false)
{
	m_successor.set = false;
	m_successor.pending = false;
	m_successor.heartbeat = CHORD_DEFAULT_HEART_BEAT;
	m_successor.resiliancy = CHORD_DEFAULT_RESILLIANCY;
	m_successor.missed = false;
	m_predecessor.set = false;
	m_predecessor.pending = false;
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


	Request get_request;
	std::string get_message;
	get_request.set_id(Hash(Name).raw());
	get_request.set_forward(false);
	get_request.set_type(Request::GET);
	get_request.SerializeToString(&get_message);
	m_primary_socket->write(get_message, m_successor.ip, m_successor.port);

	// Now sleep until you are awoken
	std::unique_lock<std::mutex> lock(m_wait_mux);
	m_wait_cv.wait(lock);
	ip = m_lookup_ip;
	port = m_lookup_port;

	m_lookup_ip = "";
	m_lookup_port = 0;

	if (ip == "" || port == 0) return 1;
	return 0;
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

	dump_log.write("Predecessor:");
	if (m_predecessor.set)
	{
		dump_log.write(m_predecessor.uid_hash.toString());
		dump_log.write("IP: " + m_predecessor.ip);
		dump_log.write("Port: " + std::to_string(m_predecessor.port) + '\n');
	}
	else dump_log.write("No predecessor\n");

}


// Network stuff
void ChordDNS::create(unsigned short port)
{
	m_port = port;
	for (unsigned int i = 0; i < CHORD_DEFAULT_HANDLER_THREADS; ++i)
		m_handler_threads.push_back(
				std::thread(&ChordDNS::request_handler, this));
	m_primary_socket = new UDPSocket(port);
	m_predecessor.set = false;
}

// Join methods
void ChordDNS::join(const std::string& host_ip,
		unsigned short host_port, unsigned short my_port)
{
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
		switch(current_request.type())
		{
			case Request::JOIN:
				{
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
					handle_get(current_request, client_ip,
							client_port);
				}
				break;
			case Request::RES:
				{
					handle_res(current_request, client_ip,
							client_port);
				}
				break;
			case Request::BAD:
				{
					handle_bad(current_request, client_ip,
							client_port);
				}
				break;
			default:
				{
					std::cerr <<"Request Protocol Error: type"
						<< current_request.type() << '\n';
					// Wake thread
					m_lookup_ip = "";
					m_lookup_port = 0;
					m_wait_cv.notify_all();
				}
				break;
		}
	}
}

void ChordDNS::handle_join(const Request& req, const std::string& ip,
		unsigned short port)
{
	Hash client_hash(req.id(), true);
	if (!m_successor.set)
	{
		// Configure struct
		m_successor.set = true;
		m_successor.resiliancy = CHORD_DEFAULT_RESILLIANCY;
		m_successor.heartbeat = CHORD_DEFAULT_HEART_BEAT;
		m_successor.missed = false;

		// Client configurations
		m_successor.uid_hash = client_hash;
		if (req.port() != 0) m_successor.port = req.port();
		else m_successor.port = port;
		if (req.forward()) m_successor.ip = req.ip();
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
		}
		else
		{
			std::cout << " duplicate domain name\n";
		}
	}
	else if (client_hash.between(m_uid_hash, m_successor.uid_hash))
	{
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
		if (req.forward()) m_successor.port = req.port();
		else m_successor.port = port;
		m_successor.uid_hash = client_hash;
	}
	else
	{
		Request forwarded_join;
		std::string join_message;
		forwarded_join.set_id(client_hash.raw());

		if (req.forward()) forwarded_join.set_ip(req.ip());
		else forwarded_join.set_ip(ip);
		if (req.forward()) forwarded_join.set_port(req.port());
		else forwarded_join.set_port(port);

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
	if (!m_successor.set)
	{
		Request response;
		std::string response_message;
		response.set_id(req.id());
		response.set_forward(false);
		response.set_type(Request::BAD);
		response.SerializeToString(&response_message);
		if (req.forward()) m_primary_socket->write(response_message,
				req.ip(), req.port());
		else m_primary_socket->write(response_message, ip, port);
	}
	else if (search_hash == m_successor.uid_hash)
	{
		Request response;
		std::string response_message;
		response.set_id(req.id());
		response.set_forward(false);
		response.set_type(Request::RES);
		response.set_ip(m_successor.ip);
		response.set_port(m_successor.port);
		response.SerializeToString(&response_message);

		if (req.forward()) m_primary_socket->write(response_message,
					req.ip(), req.port());
		else m_primary_socket->write(response_message, ip, port);
	}
	else if (search_hash == m_uid_hash)
	{
		std::cout << "Error: Landed on node\n";
		/*m_chord_log.write("Error: Landed on node (" + ip +
				":" + std::to_string(port)); */
	}
	else if (search_hash.between(m_uid_hash, m_successor.uid_hash))
	{
		Request response;
		std::string response_message;
		response.set_id(req.id());
		response.set_forward(false);
		response.set_type(Request::BAD);
		response.SerializeToString(&response_message);
		if (req.forward()) m_primary_socket->write(response_message,
				req.ip(), req.port());
		else m_primary_socket->write(response_message, ip, port);
	}
	else
	{
		// Forwarded Request
		Request get_request;
		std::string get_message;
		get_request.set_id(req.id());
		get_request.set_type(Request::GET);
		if (req.forward()) get_request.set_ip(req.ip());
		else get_request.set_ip(ip);
		if (req.forward()) get_request.set_port(req.port());
		else get_request.set_port(port);
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
	if (req.forward() == true)
	{
		Hash client_hash(req.id(), true);
		m_successor.missed = false;
		if (client_hash != m_uid_hash)
		{
			//std::cout << " Setting new successor\n";
			// Update my successor to the predecessor
			m_successor.set = true;
			m_successor.ip = req.ip();
			m_successor.port = req.port();
			m_successor.uid_hash = client_hash;

			std::cout << "New Successor: " <<
				m_successor.uid_hash.toString() << '\n' << '\t'
				<< m_successor.ip << ':' << m_successor.port
				<< '\n' << '\n';
		}

	}
	else // Handle sync request
	{
		Request sync_request;
		std::string sync_message;

		Hash client_hash(req.id(), true);

		// Use sync as stabilizer
		// in chord algorithm
		// (notify n')
		if (!m_predecessor.set ||
				client_hash.between(m_predecessor.uid_hash,
					m_uid_hash))
		{
			// You are now!

			m_predecessor.set = true;
			m_predecessor.pending = false;
			// Necessary?
			m_predecessor.resiliancy = CHORD_DEFAULT_RESILLIANCY;
			m_predecessor.heartbeat = CHORD_DEFAULT_HEART_BEAT;
			// SYNC requests are never forwarded so this is safe
			m_predecessor.ip = ip;
			m_predecessor.port = port;
			m_predecessor.uid_hash = client_hash;
			std::cout << "Predecessor: " <<
				m_predecessor.uid_hash.toString() << '\n' << '\t'
				<< m_predecessor.ip << ':' << m_predecessor.port
				<< '\n' << '\n';
		}
		else if (!m_successor.set)
		{
			m_successor.set = true;
			m_successor.pending = false;
			m_successor.resiliancy = CHORD_DEFAULT_RESILLIANCY;
			m_successor.heartbeat = CHORD_DEFAULT_HEART_BEAT;
			m_successor.ip = ip;
			m_successor.port = port;
			m_successor.uid_hash = client_hash;
			std::cout << " Fixing Ring: " <<
				m_successor.uid_hash.toString() << '\n';
		}
		else if (m_predecessor.uid_hash == client_hash)
		{
			m_predecessor.pending = false;
		}
		else
		{
			// Okay, this is the wrong guy, maybe...
			// We need to urgently send a request to our
			// predecessor to see if it is still alive.
			// If it isn't, let the new node be our predecessor
			if (m_predecessor.pending)
			{
				// Okay, pretty sure that there is an issue
				// with our predecessor
				std::cout << " Predecessor appears dead\n";
				// We replace our predecessor ? Or we could
				// send a pred request just in case and the
				// next time they come around, we replace them

				// TODO pred request before replacement
				m_predecessor.uid_hash = client_hash;
				m_predecessor.ip = ip;
				m_predecessor.port = port;
				m_predecessor.set = true;
				m_predecessor.pending = false;
				m_predecessor.missed = false;

			//	std::cout << " New Predecessor:\n"
			//		<< m_predecessor.uid_hash.toString()
			//		<< ' '
			//		<< m_predecessor.ip << ':'
			//		<< m_predecessor.port << '\n';
			}
			else
			{
				std::cout << " Predecessor is now pending...\n";
				m_predecessor.pending = true;
			}
		}
		// Build sync request
		sync_request.set_id(m_predecessor.uid_hash.raw());
		sync_request.set_type(Request::SYNC);
		sync_request.set_ip(m_predecessor.ip);
		sync_request.set_port(m_predecessor.port);
		sync_request.set_forward(true);
		sync_request.SerializeToString(&sync_message);
		m_primary_socket->write(sync_message, ip, port);
	}
}

void ChordDNS::handle_res(const Request& req, const std::string& ip,
		unsigned short port)
{
	m_lookup_ip = req.ip();
	m_lookup_port = req.port();
	m_wait_cv.notify_all();
}

void ChordDNS::handle_bad(const Request& req, const std::string& ip,
		unsigned short port)
{
	m_lookup_ip = "";
	m_lookup_port = 0;
	m_wait_cv.notify_all();
}

void ChordDNS::handle_pred(const Request& req, const std::string& ip,
		unsigned short port)
{
	if (m_predecessor.set)
	{

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
			//m_successor.pending = true;
			if (m_predecessor.set)
			{
				// Send a SYNC request to our predecessor
				m_primary_socket->write(sync_message,
						m_predecessor.ip,
						m_predecessor.port);
			}
			else std::cout << " Chord ring failed\n";

		}
		else m_successor.resiliancy--;
		m_successor.missed = true;
	}
	else if (m_predecessor.set) // Try to connect to our predecessor
	{
		std::cout << " Requesting reconnect at: "
			<< m_predecessor.uid_hash.toString() << ' '
			<< m_predecessor.ip << ':' << m_predecessor.port
			<< '\n';
		Request sync_request;
		std::string sync_message;
		sync_request.set_id(m_uid_hash.raw());
		sync_request.set_type(Request::SYNC);
		sync_request.set_forward(false);
		sync_request.SerializeToString(&sync_message);
		m_primary_socket->write(sync_message,
				m_predecessor.ip,
				m_predecessor.port);
	}
}

void ChordDNS::heartBeat()
{
	while (!m_dead)
	{
		pulse();
		if (m_predecessor.pending || !m_predecessor.set ||
				m_successor.pending) usleep (150);
		else usleep(m_successor.heartbeat * 1000);
	}
}
