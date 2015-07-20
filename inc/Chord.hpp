/*
 * Chord
 * File:     Chord.hpp
 * Author:   Evan Wilde                    <etcwilde@uvic.ca>
 * Date:     Jun 02 2015
 */

#ifndef CHORD_HPP
#define CHORD_HPP

#include <iomanip>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <utility>
#include <vector>
#include <condition_variable>
#include <mutex>

#include <unistd.h>
#include <string.h>

#include "chord_message.pb.h"
#include "Socket.hpp"
#include "logging.hpp"
#include "Hash.hpp"

#define CHORD_DEFAULT_PORT 1994 	// Default connection port if no port supplied
#define CHORD_DEFAULT_HEART_BEAT 3000 	// Ping every 3 seconds
#define CHORD_DEFAULT_RESILLIANCY 3 	// Three pulse misses and it is dead
#define CHORD_DEFAULT_HANDLER_THREADS 1

// We use a heartbeat to determine if the node is still alive. When three
// pulses are missed, the node is dead and should be removed from the chord
// ring

// WHAT HAVE I DONE THIS ISN'T SUPPOSED TO BE DEPRESSING...

// We store the information about or neighbors
//


//std:tuple<std::thread, std::string&, unsigned short&> lookup

namespace DNS
{
	class ChordDNS
	{
	public:
		ChordDNS(const std::string& DomainName);
		~ChordDNS();

		// Return 0 if no error
		int Lookup(const std::string& Name,
				std::string& ip, unsigned short& port);

		/*
		 * <UID>::<UID_HASH>
		 *
		 * Successor:
		 * <Successor UID_HASH>
		 * <Successor IP>:<Successor Port>
		 */
		void Dump(const std::string& dump_name);
	private:

		static inline size_t hashFunction(const Hash& h)
		{ return h.djb2(); }

		// Temporary method of mapping backward
		std::mutex m_wait_mux;
		std::condition_variable m_wait_cv;

		std::string m_lookup_ip;
		unsigned short m_lookup_port;


		std::unordered_map<Hash,
			std::pair< std::condition_variable,
				std::vector<
					std::pair<
					std::string&,
					unsigned short&> > >,
				std::function<decltype(hashFunction)> > m_lookups;

	// Chord Stuff -- Interface with the networking side
	public:
		// Create a chord ring
		void create(unsigned short port=CHORD_DEFAULT_PORT);
		// Join a Chord Ring
		void join(const std::string& host_ip,
				unsigned short host_port=CHORD_DEFAULT_PORT,
				unsigned short my_port=CHORD_DEFAULT_PORT);
	protected:
		// Request handlers
		void handle_join(const Request& req, const std::string& ip,
				unsigned short port);
		void handle_get(const Request& req, const std::string& ip, unsigned short port);
		void handle_set(const Request& req, const std::string& ip, unsigned short port);
		void handle_sync(const Request& req, const std::string& ip, unsigned short port);
		void handle_res(const Request& req, const std::string& ip, unsigned short port);
		void handle_bad(const Request& req, const std::string& ip, unsigned short port);
		void handle_pred(const Request& req, const std::string& ip, unsigned short port);


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
		//void stabilize();
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
			bool pending;
			bool missed;
			unsigned int resiliancy;
			double heartbeat;
			std::string ip;
			unsigned short port;
			Hash uid_hash;
			//Peer peer;
		} neighbor_t;

		neighbor_t m_successor;
		neighbor_t m_predecessor;
		neighbor_t m_predecessor_cached;
	private: // Extras
		//Log m_chord_log;
	};
};

#endif//CHORD_HPP
