#ifndef CHORD_DAEMON_HPP
#define CHORD_DAEMON_HPP

#include <condition_variable>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <mutex>
#include <regex>
#include <thread>
#include <tuple>
#include <utility>
#include <string>

#include <unistd.h>
#include <string.h>

#include "Chord.hpp"

namespace Chord
{
	class daemon
	{
		// Okay, now lets play!
		public:
			// Need
			// Name
			// RPC Port
			// IPC socket file name
			// Hosts files
			daemon(const std::string& hostname,
					const std::string& socket_file,
					const unsigned int& port,
					const std::string& hostfile);

			// Destroy socket file would probably be a good thing
			//~daemon();

			// Sets the list of hosts that we can use
			const std::vector<std::tuple< std::string, std::string, unsigned int> >& hosts()const;
		protected:
		private:
			const std::string& m_hostname;
			const std::string& m_sockfilename;
			const unsigned int& m_port;
			const std::string& m_hostfilename;

			// Rebuild mapping every time we update the file
			// (IP address / Hostname / Seek Position
			std::vector<std::tuple<
				std::string,
				std::string,
				unsigned int>> m_hosts;
	};
}

#endif//CHORD_DAEMON_HPP
