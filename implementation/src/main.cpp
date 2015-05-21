#include <iostream>
#include <string>

#include "Socket.hpp"

int main()
{
#ifdef CLIENT
	std::cout << "Client\n";
	int socket_port;
	std::cout << "Port Number: ";
	std::cin >> socket_port;
	std::cout << "Port number set: " << socket_port << '\n';
	RDSocket sock(socket_port);
	std::string message;
	std::string ip_address;
	std::cout << "Server ip address: ";
	std::cin >> ip_address;
	std::cout << "Sending to: " << ip_address << '\n';

	while (std::cin)
	{
		std::cout << "Prompt: ";
		getline(std::cin, message);
		if (message == "") continue;
		std::cout << "Sending: " << message << "...\n";
		sock.write(message, ip_address, 8080);
	}
#endif

#ifdef SERVER
	std::cout << "Server\n";
	RDSocket sock(8080);
	std::string message;
	std::string client_ip;
	unsigned short client_port;
	do
	{
		message = std::string();
		sock.read(message, client_ip, client_port);
		std::cout << client_ip << ":" << client_port << "$ " <<
				message << '\n';

	}
	while (message.compare("q") != 0);
#endif
	return 0;
}
