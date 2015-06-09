
/* * Chord
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
	m_heartbeat = 1000;
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
	std::cout << " Creating Chord Node\n";
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
		unsigned short host_port, unsigned short my_port)
{
	std::cout << " Joining Chord Node\n";
	create(my_port);
	// Now send join request
	ChordDHT::Request join_request;
	std::string join_message;
	// TODO: Use a thing specific to this node, the port number will only
	// be unique for nodes on the same machine
	Hash my_hash(std::to_string(my_port));
	join_request.set_id(my_hash.toString());
	join_request.set_content("CHORD JOIN <my_ip>--<my_port>");
	join_request.set_type(ChordDHT::Request::JOIN);
	join_request.SerializeToString(&join_message);
	m_primary_socket->write(join_message, host_ip, host_port);
}

void Chord::request_processor()
{
	std::cout << " Request Processor made!\n";

	while (m_primary_socket == NULL) {} //Spin lock
	while (!m_dead)
	{
		std::cout << "  Making request, string, and ip stuff\n";
		ChordDHT::Request current_request;
		std::string message;
		std::string client_ip;
		unsigned short client_port;
		std::cout << "  Made...\n  Reading from socket\n";
		m_primary_socket->read(message, client_ip, client_port);
		if (m_dead) break;
		current_request.ParseFromString(message);
		// Okay cool, we now have the request information
		std::cout << "Request: " <<  current_request.id() << ":"
			<< current_request.content() << '\n';
	}
	std::cout << " Request Processor killed!\n";
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





