
/*
 * main
 * File:     main.cpp
 * Author:   Evan Wilde                    <etcwilde@uvic.ca>
 * Date:     Jun 07 2015
 */

#include <iostream>
#include <string>

#include "Socket.hpp"
#include "ChordKey.hpp"
#include "Chord.hpp"
#include "chord_message.pb.h"
#include "HashTable.hpp"

#define DEFAULT_PORT 8080

int main(int argc, const char* argv[])
{

	ChordDHT::Chord chord_test;
	std::string client_ip;
	unsigned short client_port;
	unsigned short host_port;

	GOOGLE_PROTOBUF_VERIFY_VERSION;
	if (argc != 2 && argc != 4)
	{
		std::cout << "Usage: " << argv[0] << " <My Port> <Host IP address> <Host Port Number>\n";
		return 1;
	}

	host_port = static_cast<unsigned short>(atoi(argv[1]));
	// We are alone in the world, we just sit here and be quite
	if (argc == 2)
	{
		chord_test.create(host_port);
	}
	else // We have the host IP and port number
	{
		client_ip = std::string(argv[2]);
		client_port = static_cast<unsigned short>(atoi(argv[3]));
		chord_test.join(client_ip, client_port, host_port);
	}
	std::cout << " Hit Return to exit\n";
	std::string something;
	std::getline(std::cin, something);
	//std::cin >> something;
	return 0;
}
