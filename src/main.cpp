
/*
 * main
 * File:     main.cpp
 * Author:   Evan Wilde                    <etcwilde@uvic.ca>
 * Date:     Jun 07 2015
 */

#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <tuple>
#include <vector>
#include <regex>

#include "Chord.hpp"
#include "HashTable.hpp"
#include "Socket.hpp"
#include "chord_message.pb.h"
#include "logging.hpp"


#ifdef BUILD_CLIENT
#else // BUILD_DAEMON
#define CONF_FILE "ddns.conf"

//#define HOSTNAME_PATTERN "^hostname[ \t]*?=[ \t]*?([0-9a-zA-Z\\.]*?)[ \t]*?(?:#@.*)?$"

#define HOSTNAME_PATTERN "^hostname[ \t]*?=[ \t]*?([0-9a-zA-Z\\.]*?)[ \t]*?(?:#.*)?$"
#define HOSTFILE_PATTERN "^hostfile[ \t]*?=[ \t]*?([0-9a-zA-Z\\/.]*?)[ \t]*?(?:#.*)?$"


// Read the configuration file
// Read hosts file
// if IP/Port passed
// 	use those to connect
// else
// 	Attempt to connect to any from hosts file
//
// Setup IPC unix sockets
// Listen for requests forever
// Service requests

int main(int argc, char** argv)
{
	// Things that need setting
	// hostsfile
	// hostname

	// Read the configuration file
	std::ifstream conf_file;
	conf_file.open(CONF_FILE);

	std::string hostname = "";
	std::string hostfile = "";

	std::string line;
	if (conf_file.is_open())
	{
		while(getline(conf_file, line))
		{
			if (line.c_str()[0] == '#') continue;
			std::string s = line;
			std::vector<std::string> matches;
			std::smatch match;
			std::regex re(HOSTNAME_PATTERN);
			while (std::regex_search(s, match, re))
			{
				for (auto x: match)  matches.push_back(x);
				s = match.suffix().str();
			}
			if (matches.size() > 0) hostname = matches[1];
			else
			{
				s = line;
				re = std::regex(HOSTFILE_PATTERN);
				while (std::regex_search(s, match, re))
				{
					for (auto x: match) matches.push_back(x);
					s = match.suffix().str();
				}
				if (matches.size() > 0) hostfile = matches[1];
			}

		}
		conf_file.close();
	}
	else
	{
		std::cerr << "Failed to open configuration file\n";
		return 1;
	}
	if (hostname.compare("") == 0)
	{
		std::cerr << "No hostname specified\n";
		return 1;
	}
	std::cout << "Hostname: " << hostname << '\n';
	std::cout << "Hostfile: " << hostfile << '\n';
	// Read hosts file


	// if IP/Port passed
	// 	use those to connect
	// else
	// 	Attempt to connect to any from hosts file

	// Setup IPC unix sockets

	// Listen for requests forever

	// Service requests
	return 0;
}
#endif
