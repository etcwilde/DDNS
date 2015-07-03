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
#include "logging.hpp"

#define CHORD_DEFAULT_PORT 1994 	// Default connection port if no port supplied
#define CHORD_DEFAULT_HEART_BEAT 3000 	// Ping every 3 seconds
#define CHORD_DEFAULT_RESILLIANCY 3 	// Three pulse misses and it is dead
#define CHORD_DEFAULT_TABLE_SIZE 512	// 512 items can be stored on a single node
//#define CHORD_DEFAULT_HANDLER_THREADS 3	// Uses 3 threads for handling by default
#define CHORD_DEFAULT_HANDLER_THREADS 1

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
		unsigned long m_local_size;

	// Chord Stuff -- Interface with the networking side
	public:
		void create(unsigned short port=CHORD_DEFAULT_PORT);
		// Join a
		void join(const std::string& host_ip,
				unsigned short host_port=CHORD_DEFAULT_PORT,
				unsigned short my_port=CHORD_DEFAULT_PORT);
	protected:
		// Request handlers
		void handle_join(const Request& req, const std::string& ip,
				unsigned short port);
		void handle_get(const Request& req, const std::string& ip,
				unsigned short port, const Hash& key);
		void handle_set(const Request& req, const std::string& ip, unsigned short port);
		void handle_sync(const Request& req, const std::string& ip, unsigned short port);

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
		unsigned int m_resiliancy;
		bool m_dead;
		std::thread m_heart;

	// Networking Stuff
	private:
		/*
		 * neighbor_t
		 * fields:
		 * 	set : Is the information in this structure valid
		 * 	missed: did the node miss the last beat?
		 * 	heartbeat: how often the neighbor needs to be queried
		 * 	resiliency: how much life is left on the neighbor
		 * 	peer: A container for the ip, port number, and hash
		 */
		typedef struct
		{
			bool set;
			bool missed;
			unsigned int resiliancy;
			double heartbeat;
			Peer peer;
		} neighbor_t;
		neighbor_t m_successor;
		neighbor_t m_predecessor;
	private: // Extras
		Log m_chord_log;
	};
};

#endif//CHORD_HPP
