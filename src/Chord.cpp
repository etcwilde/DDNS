
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
	std::cout << " Creating Chord Node\n";
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
	Hash my_hash(std::to_string(my_port));
	join_request.set_id(my_hash.toString());
	join_request.set_content(m_uid_hash.raw());
	join_request.set_type(Request::JOIN);
	join_request.SerializeToString(&join_message);
	m_primary_socket->write(join_message, host_ip, host_port);
}


// Request handling
void Chord::request_handler()
{
	std::cout << " Request Processor made!\n";
	while (m_primary_socket == NULL) {} //Spin lock
	while (!m_dead)
	{
		Request current_request;
		std::string message;
		std::string client_ip;
		unsigned short client_port;
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
	std::cout <<"JOIN: " <<  ip << ":" << port << "| " << Hash(req.content(), true).toString() << '\n';

	Hash client_hash(req.content(), true);
	//std::cout << m_uid_hash.toString() << '\n';
	//std::cout << client_hash.toString() << '\n';
	//std::cout << client_hash.compareTo(m_uid_hash) << '\n';

	if (client_hash.compareTo(m_uid_hash) == 0)
	{
		std::cout << client_hash.toString() << " : " << m_uid_hash.toString() << '\n';
		std::cout << " This ID already exists\n";
	}
	else if (!m_successor.set)
	{
		std::cout << " This node is now our successor\n";
		if (!req.pass())
		{
			m_successor.set = true;
			m_successor.peer.ip_address = ip;
			m_successor.peer.port_number = port;
			m_successor.peer.uid_hash = client_hash;
		}
		else
		{
			m_successor.set = true;
			m_successor.peer.ip_address = req.ip();
			m_successor.peer.port_number = req.port();
			m_successor.peer.uid_hash = client_hash;
		}
	}
	else if (client_hash.between(m_uid_hash, m_successor.peer.uid_hash))
	{
		std::cout << " " << ip << ":" << port << " is between\n";
		// The node goes between me and my successor

		// We need to set our successors successor the our old
		// successor
		//

		// Store old successor
		// Replace sucessor
		// Join old sucessor with new successor Node
		// It won't have a sucessor, so it will go ahead and add it
		// This seems dangerous but lets try it

		// Process here

		// Store old sucessor
		Peer old_sucessor = m_successor.peer;
		// Update sucessor
		m_successor.peer.ip_address = ip;
		m_successor.peer.port_number = port;
		m_successor.peer.uid_hash = client_hash;

		// JOIN old sucessor to new successor
		Request pass_forward;
		pass_forward.set_id(old_sucessor.uid_hash.toString());
		pass_forward.set_content(old_sucessor.uid_hash.raw());
		pass_forward.set_pass(true);
		pass_forward.set_ip(old_sucessor.ip_address);
		pass_forward.set_port(old_sucessor.port_number);
		std::string forward_message;
		pass_forward.SerializeToString(&forward_message);
		m_primary_socket->write(forward_message, m_successor.peer.ip_address,
				m_successor.peer.port_number);
	}
	else
	{
		// Pass forward one hop O(N) join
		Request pass_forward;
		pass_forward.set_id(client_hash.toString());
		pass_forward.set_content(client_hash.raw());
		pass_forward.set_pass(true);
		pass_forward.set_ip(ip);
		pass_forward.set_port(port);
		pass_forward.set_type(Request::JOIN);
		std::string forward_message;
		pass_forward.SerializeToString(&forward_message);
		m_primary_socket->write(forward_message, m_successor.peer.ip_address, m_successor.peer.port_number);
	}
	std::cout << "Successor: " << m_successor.peer.ip_address << ":" <<
		m_successor.peer.port_number << " "
		<< m_successor.peer.uid_hash.toString() << '\n';
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
	//std::cout << " ** thump **\n";
}
void Chord::heartBeat()
{
	//while (!m_dead) { pulse(); usleep(m_heartbeat * 1000); }
}





