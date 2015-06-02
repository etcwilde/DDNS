#include <iostream>
#include <string>

#include "Socket.hpp"

int main()
{
	//SCTPSocket my_socket(8080);
#ifdef CLIENT
	UDPSocket sock("142.104.68.132", 8081);
	std::string message;
	std::string ip_address;
	ip_address = "142.104.68.132";

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
	UDPSocket sock("142.104.68.132", 8080);
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
