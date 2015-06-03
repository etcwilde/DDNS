#include <iostream>
#include <string>

#include "Socket.hpp"
#include "ChordKey.hpp"
#include "Chord.hpp"

#define DEFAULT_PORT 8080

int main(int argc, const char* argv[])
{
	/*UDPSocket* socket;
	if (argc != 2 && argc != 4)
	{
		std::cout << "Usage: " << argv[0] << " <My Port> <Host IP address> <Host Port Number>\n";
		return 1;
	}

	unsigned short host_port = (unsigned short)atoi(argv[1]);
	std::string message;
	std::string client_ip;
	unsigned short client_port;
	// We are alone in the world, we just sit here and be quite
	if (argc == 2)
	{
		// We are alone and no chord
		socket = new UDPSocket(host_port);
		socket->read(message, client_ip, client_port);
		std::cout << "From: " << client_ip << ":" << client_port << "| "
			<< message << '\n';
		socket->write("Bye!", client_ip, client_port);
	}
	else
	{
		client_ip = std::string(argv[2]);
		client_port = (unsigned short)atoi(argv[3]);
		// We are not alone
		socket = new UDPSocket(host_port);
		socket->write("Hello!", client_ip, client_port);
		socket->read(message, client_ip, client_port);
		std::cout << "From: " << client_ip << ":" << client_port << "| "
			<< message << '\n';
	}

	delete socket;
	*/

	Chord<int> m_chord;

	for (unsigned int i = 0; i < DEFAULT_PORT * 100000 ; i++)
	{

		// Do a thing
	}

	return 0;
}
