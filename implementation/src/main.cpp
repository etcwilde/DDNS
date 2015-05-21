#include <iostream>
#include <string>

#include "Socket.hpp"

int main()
{

#ifdef CLIENT
	std::cout << "Client\n";

	UDPSocket sock(8081);

	std::string message;

	std::string ip_address;
	std::cout << "Server ip address: ";
	getline(std::cin, ip_address);

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
	UDPSocket sock(8080);

	std::string message;
	std::string client_ip;
	unsigned short client_port;
	do
	{
		sock.read(message, client_ip, client_port);
		std::cout << client_ip << ":" << client_port << "$ " <<
				message << '\n';

		sock.write("SERVER RESPONSE >>> MESSAGE RECVD\n",
				client_ip, client_port);

	}
	while (message != "Exit");
#endif
	return 0;
}
