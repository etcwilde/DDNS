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
	// Request get_request;
	// std::string get_message;
	// get_request.set_id(Hash(Name).raw());
	// std::cout << "Packing Search Request:\n" <<
	// 	Hash(get_request.id(), true).toString() << '\n';
	// get_request.set_forward(false);
	// get_request.set_type(Request::GET);
	// get_request.SerializeToString(&get_message);
	//m_primary_socket->write(get_message, m_successor.ip, m_successor.port);

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
}


// Request handling
void ChordDNS::request_handler()
{
	while (m_primary_socket == NULL) {} //Spin lock
	while (!m_dead)
	{

		std::string input;
		std::string client_ip;
		unsigned short client_port;
		m_primary_socket->read(input, client_ip, client_port);
		if (m_dead) break;
	}
}

void ChordDNS::handle_join(const Request& req, const std::string& ip,
		unsigned short port)
{
	Hash client_hash;
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

		// Complete Chord Ring

	}
	else if (client_hash == m_successor.uid_hash)
	{
	}
	else if (client_hash.between(m_uid_hash, m_successor.uid_hash))
	{
	}
	else
	{
	}
}

void ChordDNS::handle_get(const Request& req, const std::string& ip,
		unsigned short port)
{
}

/**
 * This will always be directly from our predecessor or successor
 * We will use the forward flag as a way of determining if it is a response
 */
void ChordDNS::handle_sync(const Request& req, const std::string& ip,
		unsigned short port)
{
	// Handle Response
}


// Heart Stuff
void ChordDNS::pulse()
{
	if (m_successor.set)
	{

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
