
/*
 * Chord
 * File:     Chord.cpp
 * Author:   Evan Wilde                    <etcwilde@uvic.ca>
 * Date:     Jun 02 2015
 */

#include "Chord.hpp"

using namespace ChordDHT;

ChordDHT::Chord::Chord(uint global_exponent, ulong local_size):
	m_global_exponent(global_exponent),
	m_local_size(local_size),
	m_primary_socket(NULL)
{
	m_dead = false;
	m_heart = std::thread(&Chord::heartBeat, this);
}

Chord::Chord::~Chord()
{
	m_dead = true;
	m_heart.join();
	if (m_primary_socket) delete m_primary_socket;
}

void Chord::Chord::insert(const Hash& key, std::string value)
{
}

bool Chord::check(const Hash& key, std::string test_value)
{
	return false;
}

std::string Chord::lookup(const Hash& key)
{
	return "Hello";
}

void Chord::remove(const Hash& key)
{
}

// Network stuff
void Chord::create(unsigned short port)
{
	m_port = port;
	// Cool maybe spin up a thread to listen for friends?
	for (unsigned int i = 0; i < CHORD_DEFAULT_HANDLER_THREADS; ++i)
	{
		m_handler_threads.push_back(
				std::thread(&Chord::request_processor, this));
	}
	m_primary_socket = new UDPSocket(port);
}

void ChordDHT::Chord::join(const std::string& host_ip,
		unsigned short host_port, short my_port)
{
	create(my_port);
	// Now send join request
	ChordDHT::Request join_request;
}

void Chord::request_processor()
{

	ChordDHT::Request current_request;
	std::string message;
	std::string client_ip;
	unsigned short client_port;
	Socket->read(messag, client_ip, client_port);
	current_request.ParseFromString(message);
	// Okay cool, we now have the request information
}

void Chord::handle_request(const ChordDHT::Request& request)
{
}




// Heart Stuff

void Chord::pulse() { std::cout << " ** thump **\n"; }
void Chord::heartBeat()
{
	while (!m_dead)
	{
		pulse();
		usleep(m_heartbeat * 1000);
	}
}





