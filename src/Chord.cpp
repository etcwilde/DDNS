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
	m_chord_log("logs/"+std::to_string(getpid())+"_chord.log")
{
	m_dead = false;
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
	create(my_port);
	// Now send join request
	Request join_request;
	std::string join_message;
	join_request.set_id(m_uid_hash.raw());
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
		m_chord_log.write("Request: "
				+ std::to_string(current_request.type())
				+ " FROM " + client_ip
				+ ":" + std::to_string(client_port));

		switch(current_request.type())
		{
			case Request::JOIN:
				{
					std::cout << "JOIN REQUEST\n";
					handle_join(current_request, client_ip,
							client_port);
				}
				break;
			case Request::DROP:
				{
					std::cout << "DROP REQUEST\n";
				}
				break;
			case Request::BAD:
				{
					std::cout << "BAD REQUEST\n";
				}
				break;
			case Request::SYNC:
				{
					handle_sync(current_request, client_ip,
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
	std::cout << " Request Processor killed!\n";
}

void ChordDNS::handle_join(const Request& req, const std::string& ip,
		unsigned short port)
{
	Hash client_hash(req.id(), true);
	if (client_hash.compareTo(m_uid_hash) == 0)
	{
		m_chord_log.write("Node with same UID: " + ip + ":"
				+ std::to_string(port));
	}
	else if (!m_successor.set)
	{
		//std::cout << " No successor set\n";
		// The ring is established, we need to join
		if (req.forward() == true)
		{
			// Just insert it and be done
			m_successor.ip = req.ip();
			m_successor.port = req.port();
			m_successor.uid_hash = Hash(req.id(), true);
		}
		else
		{
			// We don't have a successor and are getting request
			// We have to start with a chord ring to make this work
			// Accept this node as our successor
			// Then send a join to our successor so that we are the
			// successor of our successor

			// Assume for now that there is no forwarding of nodes
			m_successor.ip = ip;
			m_successor.port = port;
			m_successor.uid_hash = client_hash;

			// Create JOIN request to join with our successor and complete
			// the ring
			Request join_request;
			std::string join_message;
			join_request.set_id(m_uid_hash.raw());
			join_request.set_type(Request::JOIN);
			// We don't know our own IP address, so don't even bother
			// We don't know our own port number either...

			join_request.SerializeToString(&join_message);
			m_primary_socket->write(join_message, m_successor.ip,
					m_successor.port);
		}
		m_successor.resiliancy = CHORD_DEFAULT_RESILLIANCY;
		m_successor.set = true;
		m_successor.missed = false;
	}
	else if (client_hash.between(m_uid_hash, m_successor.uid_hash))
	{
		neighbor_t old_successor;
		old_successor.ip = m_successor.ip;
		old_successor.port = m_successor.port;
		old_successor.uid_hash = m_successor.uid_hash;

		if (client_hash == m_successor.uid_hash)
		{

			m_successor.heartbeat /= 2;

		}



		Request join_request;
		std::string join_message;
		join_request.set_type(Request::JOIN);
		join_request.set_forward(true);
		join_request.set_ip(old_successor.ip);
		join_request.set_port(old_successor.port);
		join_request.set_id(old_successor.uid_hash.raw());
		join_request.SerializeToString(&join_message);
		if (req.forward() == true)
		{
			m_primary_socket->write(join_message, req.ip(),
					req.port());
			m_successor.ip = req.ip();
			m_successor.port = req.port();
			m_successor.uid_hash = Hash(req.id(), true);
		}
		else
		{
			m_primary_socket->write(join_message, ip, port);
			m_successor.ip = ip;
			m_successor.port = port;
			m_successor.uid_hash = client_hash;
		}
	}
	else
	{
		Request join_request;
		std::string join_message;
		join_request.set_id(client_hash.raw());
		join_request.set_type(Request::JOIN);
		join_request.set_forward(true);
		if (req.forward() == true)
		{
			join_request.set_ip(req.ip());
			join_request.set_port(req.port());
			std::cout << "forwarded\n";
		}
		else
		{
			join_request.set_ip(ip);
			join_request.set_port(port);
		}

		join_request.SerializeToString(&join_message);
		m_primary_socket->write(join_message, m_successor.ip,
				m_successor.port);

	}
	if (m_successor.set)
		std::cout << m_successor.uid_hash.toString()
			<< " <- " <<
			m_uid_hash.toString() << '\n';
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
		sync_request.set_forward(false);
		sync_request.set_type(Request::SYNC);
		sync_request.SerializeToString(&sync_message);

		m_primary_socket->write(sync_message, m_successor.ip,
				m_successor.port);

		if (m_successor.missed == false)
			m_successor.resiliancy = CHORD_DEFAULT_RESILLIANCY;
		else if (m_successor.resiliancy == 0) m_successor.set = false;
		else m_successor.resiliancy--;
		m_successor.missed = true;
	}
}

void ChordDNS::heartBeat()
{
	while (!m_dead) { pulse(); usleep(m_successor.heartbeat * 1000); }
}

