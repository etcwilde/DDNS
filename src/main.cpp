
/*
 * main
 * File:     main.cpp
 * Author:   Evan Wilde                    <etcwilde@uvic.ca>
 * Date:     Jun 07 2015
 */

#include <iostream>
#include <string>

#include "Socket.hpp"
#include "Chord.hpp"
#include "chord_message.pb.h"
#include "HashTable.hpp"

#define DEFAULT_PORT 8080

int main(int argc, const char* argv[])
{

	std::string client_ip;
	unsigned short client_port;
	std::string host_name;
	unsigned short host_port;

	GOOGLE_PROTOBUF_VERIFY_VERSION;
	if (argc != 3 && argc != 5)
	{
		std::cout << "Usage: " << argv[0] << " <My Name> <My Port> [<Host IP address> <Host Port Number>]\n";
		return 1;
	}

	host_name = std::string(argv[1]);
	host_port = static_cast<unsigned short>(atoi(argv[2]));
	std::cout << "Host Name: " << host_name << ":" << host_port << '\n';

	ChordDHT::Chord chord_test(host_name);

	// We are alone in the world, we just sit here and be quite
	if (argc == 3)
	{
		chord_test.create(host_port);
	}
	else // We have the host IP and port number
	{
		client_ip = std::string(argv[3]);
		client_port = static_cast<unsigned short>(atoi(argv[4]));
		chord_test.join(client_ip, client_port, host_port);
	}
	std::cout << " Hit Return to exit\n";
	std::string something;
	std::getline(std::cin, something);
	//std::cin >> something;
	return 0;
}
