
/*
 * Chord
 * File:     Chord.cpp
 * Author:   Evan Wilde                    <etcwilde@uvic.ca>
 * Date:     Jun 02 2015
 */

#include "Chord.hpp"

using namespace ChordDHT;

Chord::Chord(std::string uid, uint global_exponent, ulong local_size):
	m_table(),
	m_global_exponent(global_exponent),
	m_local_size(local_size),
	m_primary_socket(NULL),
	m_uid(uid),
	m_uid_hash(Hash(uid))
{
	m_dead = false;
	m_successor.set = false;
	m_heartbeat = CHORD_DEFAULT_HEART_BEAT;
	m_heart = std::thread(&Chord::heartBeat, this);
	std::cout << "Starting Chord\n";
}

Chord::Chord::~Chord()
{
	std::cout << "Ending Chord\n";
	m_dead = true;
	m_heart.join();
	m_primary_socket->shutdown();
	for (unsigned int i = 0; i < CHORD_DEFAULT_HANDLER_THREADS; ++i)
		m_handler_threads[i].join();
	if (m_primary_socket) delete m_primary_socket;
}

void Chord::Chord::insert(const Hash& key, std::string value)
{
}


bool Chord::check(const Hash& key, std::string test_value, std::string& ip,
		unsigned short& port)
{
	return false;
}

std::string Chord::lookup(const Hash& key, std::string& ip, unsigned short& port)
{
	return "Hello";
}



void Chord::remove(const Hash& key)
{
}

// Network stuff
void Chord::create(unsigned short port)
{
	std::cout << "Creating Chord Node\n";
	m_port = port;
	// Cool maybe spin up a thread to listen for friends?
	for (unsigned int i = 0; i < CHORD_DEFAULT_HANDLER_THREADS; ++i)
	{
		m_handler_threads.push_back(
				std::thread(&Chord::request_handler, this));
	}
	m_primary_socket = new UDPSocket(port);
}

void Chord::join(const std::string& host_ip,
		unsigned short host_port, unsigned short my_port)
{
	std::cout << " Joining Chord Node\n";
	std::cout << "My UID/HASH\n UID: " << m_uid << "\n HASH: " << m_uid_hash.toString() << '\n';
	create(my_port);
	// Now send join request
	Request join_request;
	std::string join_message;
	// TODO: Use a thing specific to this node, the port number will only
	// be unique for nodes on the same machine
	//Hash my_hash(std::to_string(my_port));
	join_request.set_id(m_uid_hash.raw());
	//join_request.set_content(m_uid_hash.raw());
	join_request.set_type(Request::JOIN);
	join_request.SerializeToString(&join_message);
	m_primary_socket->write(join_message, host_ip, host_port);
}


// Request handling
void Chord::request_handler()
{
	//std::cout << " Request Processor made!\n";
	while (m_primary_socket == NULL) {} //Spin lock
	while (!m_dead)
	{
		Request current_request;
		std::string message;
		std::string client_ip;
		unsigned short client_port;
		std::cout << "Waiting for message...\n";
		m_primary_socket->read(message, client_ip, client_port);
		if (m_dead) break;
		std::cout << "Message received from: " << client_ip << ":" << client_port << '\n';
		current_request.ParseFromString(message);
		switch(current_request.type())
		{
			case Request::JOIN:
				{
					handle_join(current_request,
							client_ip, client_port);
					break;
				}
			case Request::DROP:
				{
					std::cout << "DROP REQUEST\n";
					break;
				}
			case Request::GET:
				{
					std::cout << "GET REQUEST\n";
					break;
				}
			case Request::SET:
				{
					std::cout << "SET REQUEST\n";
					break;
				}
			case Request::SYNC:
				{
					std::cout << "SYNC REQUEST\n";
					break;
				}
			default:
				{
					std::cerr <<"Request Protocol Error\n";
					break;
				}
		}
	}
	std::cout << " Request Processor killed!\n";
}

void Chord::handle_join(const Request& req, const std::string& ip,
		unsigned short port)
{
	Hash client_hash(req.id(), true);
	std::cout << "(cli)" << client_hash.toString()
			<< " : (host)" << m_uid_hash.toString() << '\n';

	if (client_hash.compareTo(m_uid_hash) == 0)
	{
		std::cout << " This ID already exists\n";
	}
	else if (!m_successor.set)
	{
		std::cout << " No successor set\n";
		// The ring is established, we need to join
		if (req.pass() == true)
		{
			// Just insert it and be done
			m_successor.peer.ip = req.ip();
			m_successor.peer.port = req.port();
			m_successor.peer.uid_hash = Hash(req.id(), true);
			m_successor.set = true;
		}
		else
		{
			// We don't have a successor and are getting request
			// We have to start with a chord ring to make this work
			// Accept this node as our successor
			// Then send a join to our successor so that we are the
			// successor of our successor

			// Assume for now that there is no forwarding of nodes
			m_successor.peer.ip = ip;
			m_successor.peer.port = port;
			m_successor.peer.uid_hash = client_hash;

			// Create JOIN request to join with our successor and complete
			// the ring
			Request join_request;
			std::string join_message;
			join_request.set_id(m_uid_hash.raw());
			join_request.set_type(Request::JOIN);
			// We don't know our own IP address, so don't even bother
			// We don't know our own port number either...

			join_request.SerializeToString(&join_message);
			m_primary_socket->write(join_message, m_successor.peer.ip,
					m_successor.peer.port);
			std::cout << "Successor: " << m_successor.peer.ip << ":" <<
				m_successor.peer.port << " "
				<< m_successor.peer.uid_hash.toString() << '\n';
			m_successor.set = true;
		}
	}
	else if (client_hash.between(m_uid_hash, m_successor.peer.uid_hash))
	{

		std::cout << " Between my successor and me: ";
		if (req.pass() == true)
		{
			std::cout << "pass\n";
			Peer old_successor;
			old_successor.ip = m_successor.peer.ip;
			old_successor.port = m_successor.peer.port;
			old_successor.uid_hash = m_successor.peer.uid_hash;

			// Joing old successor to new successor
			Request join_request;
			std::string join_message;
			join_request.set_id(old_successor.uid_hash.raw());
			join_request.set_type(Request::JOIN);
			join_request.set_pass(true);
			join_request.set_ip(old_successor.ip);
			join_request.set_port(old_successor.port);
			join_request.SerializeToString(&join_message);
			m_primary_socket->write(join_message,
					req.ip(), req.port());
			m_successor.peer.ip = req.ip();
			m_successor.peer.port = req.port();
			m_successor.peer.uid_hash = Hash(req.id(), true);
		}
		else
		{
			std::cout << "direct\n";
			Peer old_successor;

			old_successor.ip = m_successor.peer.ip;
			old_successor.port = m_successor.peer.port;
			old_successor.uid_hash = m_successor.peer.uid_hash;

			// Join our old successor the the new successor
			Request join_request;
			std::string join_message;
			join_request.set_id(old_successor.uid_hash.raw());
			join_request.set_type(Request::JOIN);
			join_request.set_pass(true);
			join_request.set_ip(old_successor.ip);
			join_request.set_port(old_successor.port);

			join_request.SerializeToString(&join_message);
			std::cout << "Sending Join Message for my old successor to: "
				<< ip << ":" << port << '\n';
			m_primary_socket->write(join_message, ip, port);

			m_successor.peer.ip = ip;
			m_successor.peer.port = port;
			m_successor.peer.uid_hash = client_hash;
		}
	}
	else
	{
		std::cout << " Forward JOIN: ";
		if (req.pass() == true)
		{
			std::cout << "passed\n";
			// I think that this is a node that we don't
			std::cerr << " ------ NOT IMPLEMENTED2!!! ------\n";
			Request join_request;
			std::string join_message;

			join_request.set_id(client_hash.raw());
			join_request.set_ip(req.ip());
			join_request.set_port(req.port());
			join_request.set_type(Request::JOIN);
			join_request.set_pass(true);
			join_request.SerializeToString(&join_message);
			m_primary_socket->write(join_message,
					m_successor.peer.ip,
					m_successor.peer.port);
		}
		else
		{
			std::cout << "direct\n";

			// We will pass the request forward to our successor
			// This may create an infinite loop (Actually, it probably will
			// just watch)

			std::cout << " Hash(host):(client): " << m_uid_hash.toString() << ":" <<
				client_hash.toString() << '\n';

			Request join_request;
			std::string join_message;
			join_request.set_id(client_hash.raw());
			join_request.set_ip(ip);
			join_request.set_port(port);
			join_request.set_pass(true);
			join_request.set_type(Request::JOIN);
			join_request.SerializeToString(&join_message);
			m_primary_socket->write(join_message, m_successor.peer.ip,
					m_successor.peer.port);
		}
	}
}

void Chord::handle_drop(const Request& req, const std::string& ip,
		unsigned short port)
{
	if (!m_successor.set)
	{
		std::cout << " Error: Drop request, but we don't know who\n";
		return;
	}
	else
	{
		std::cout << " Drop request handled... Once I figure out what I'm doing\n";
	}
}

// Heart Stuff

void Chord::pulse()
{
	std::cout << " ** thump **\n";
	std::cout << "(succ)" << m_successor.peer.uid_hash.toString()
	       << " : (host)" << m_uid_hash.toString() << '\n';
	std::cout << " ** thump **\n";

}
void Chord::heartBeat()
{
	while (!m_dead) { pulse(); usleep(m_heartbeat * 1000); }
}





