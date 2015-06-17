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
#include <iostream>

#include <unistd.h>
#include <string.h>

#include "HashTable.hpp"
#include "chord_message.pb.h"
#include "Socket.hpp"
#include "Peer.hpp"

#define CHORD_DEFAULT_PORT 1994 	// Default connection port if no port supplied
#define CHORD_DEFAULT_HEART_BEAT 3000 	// Ping every 3 seconds
#define CHORD_DEFAULT_RESILLIANCY 3 	// Three pulse misses and it is dead
#define CHORD_DEFAULT_NODES_EXPONENT 3  // Chord ring size is 3
#define CHORD_DEFAULT_TABLE_SIZE 512	// 512 items can be stored on a single node
//#define CHORD_DEFAULT_HANDLER_THREADS 3	// Uses 3 threads for handling by default
#define CHORD_DEFAULT_HANDLER_THREADS 1

#define CHORD_DEFAULT_SIZE 8 		// Number of nodes (low for testing)

// We use a heartbeat to determine if the node is still alive. When three
// pulses are missed, the node is dead and should be removed from the chord
// ring

// WHAT HAVE I DONE THIS ISN'T SUPPOSED TO BE DEPRESSING...

// We store the information about or neighbors
//

namespace ChordDHT
{
	class Chord : public DistributedHashTable
	{
	// DHT stuff -- This is the interface for the hash table
	public:
		Chord(std::string uid,
				unsigned int global_exponent=CHORD_DEFAULT_NODES_EXPONENT,
				unsigned long local_size=CHORD_DEFAULT_TABLE_SIZE);
		~Chord();

		virtual void insert(const Hash& key, std::string value);
		virtual bool check(const Hash& key, std::string test_value,
				std::string& ip, unsigned short& port);

		virtual std::string lookup(const Hash& key, std::string& ip,
				unsigned short& port);

		virtual void remove(const Hash& key);

	private:
		LocalHashTable m_table;
		unsigned int m_global_exponent;
		unsigned long m_local_size;

	// Chord Stuff -- Interface with the networking side
	public:
		void create(unsigned short port=CHORD_DEFAULT_PORT);
		// Join a
		void join(const std::string& host_ip,
				unsigned short host_port=CHORD_DEFAULT_PORT,
				unsigned short my_port=CHORD_DEFAULT_PORT);
	protected:

		// Data passers
		// Passes the join request to the next best node in the ring
		void pass_join(const Request& req);

		void pass_get(const Request& req);

		void pass_set(const Request& req);

		void pass_drop(const Request& req);


		// Request handlers
		void handle_join(const Request& req, const std::string& ip,
				unsigned short port);
		void handle_get(const Request& req, const std::string& ip,
				unsigned short port, const Hash& key);
		void handle_set(const Request& req, const std::string& ip, unsigned short port);
		void handle_drop(const Request& req, const std::string& ip, unsigned short port);

	private:
		void request_handler();
		unsigned short m_port;
		std::vector<std::thread> m_handler_threads;
		UDPSocket* m_primary_socket;
		std::string m_uid;
		Hash m_uid_hash;

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
		typedef struct
		{
			bool set;
			Peer peer;
			//std::string ip_address;
			//unsigned short port_number;

			//Hash uid_hash;
			double heartbeat;
			unsigned int resiliancy;
			bool dead;
		} neighbor_t;

		neighbor_t m_successor;
		//neighbor_t m_predecessor; // Can we get away with not having
		//a predecessor? -- Singly-circularly linked list style?
	};
};


#endif//CHORD_HPP
