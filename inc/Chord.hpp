
/*
 * Chord
 * File:     Chord.hpp
 * Author:   Evan Wilde                    <etcwilde@uvic.ca>
 * Date:     Jun 02 2015
 */

#ifndef CHORD_HPP
#define CHORD_HPP

#include <string>
#include <mutex>
#include <thread>
#include <vector>

#include <unistd.h>

#include "HashTable.hpp"
#include "chord_message.pb.h"
#include "Socket.hpp"
//#include "chord_message.pb.h"

#define CHORD_DEFAULT_PORT 1994 	// Default connection port if no port supplied
#define CHORD_DEFAULT_HEART_BEAT 1000 	// Ping every 1 seconds
#define CHORD_DEFAULT_RESILLIANCY 3 	// Three pulse misses and it is dead
#define CHORD_DEFAULT_NODES_EXPONENT 3  // Chord ring size is 3
#define CHORD_DEFAULT_TABLE_SIZE 512	// 512 items can be stored on a single node
//#define CHORD_DEFAULT_HANDLER_THREADS 3	// Uses 3 threads for handling by default
#define CHORD_DEFAULT_HANDLER_THREADS 1

// We use a heartbeat to determine if the node is still alive. When three
// pulses are missed, the node is dead and should be removed from the chord
// ring

// WHAT HAVE I DONE THIS ISN'T SUPPOSED TO BE DEPRESSING...

// We store the information about or neighbors
//

// typedef uint unsigned int;
// typedef ulong unsigned long;

namespace ChordDHT
{
	class Chord : public HashTable
	{

	// DHT stuff -- This is the interface for the hash table workd
	public:
		Chord(uint global_exponent=CHORD_DEFAULT_NODES_EXPONENT,
				ulong local_size=CHORD_DEFAULT_TABLE_SIZE);
		~Chord();

		virtual void insert(const Hash& key, std::string value);
		virtual bool check(const Hash& key, std::string test_value);
		virtual std::string lookup(const Hash& h);
		virtual void remove(const Hash&h);

	private:
		LocalHashTable m_table;
		unsigned int m_global_exponent;
		unsigned long m_local_size;

	// Chord Stuff -- Interface with the networking side
	public:
		void create(unsigned short port=CHORD_DEFAULT_PORT);
		void join(const std::string& host_ip,
				unsigned short host_port=CHORD_DEFAULT_PORT,
				unsigned short my_port=CHORD_DEFAULT_PORT);


	protected:
		void handle_join();
		void handle_get(const Hash& key);
		void handle_set(const Hash& key, std::string value);
		void handle_drop();
		void handle_request(const ChordDHT::Request& request);

	private:
		void request_processor();
		unsigned short m_port;
		std::vector<std::thread> m_handler_threads;
		UDPSocket* m_primary_socket;

	// Heartbeat stuff
	protected:
		virtual void pulse();

	private:
		void heartBeat();
		double m_heartbeat;
		unsigned int m_resiliancy;
		bool m_dead;
		std::thread m_heart;

	// Networking Stuff
	private:

	};
};


#endif//CHORD_HPP
