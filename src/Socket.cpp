
/*
 * Socket
 * File:     Socket.cpp
 * Author:   Evan Wilde                    <etcwilde@uvic.ca>
 * Date:     May 20 2015
 */

#include "Socket.hpp"

Socket::Socket(unsigned short port) :
	m_bound(false),
	m_good(false),
	m_port(port)
{
	bzero((char*)&m_address, sizeof(struct sockaddr_in));
	m_address.sin_family = AF_INET;
	m_address.sin_addr.s_addr = INADDR_ANY;
	m_address.sin_port = htons(m_port);
}

Socket::Socket(std::string host_ip, unsigned short port) :
	m_bound(false),
	m_good(false),
	m_port(port)
{
	bzero((char*)&m_address, sizeof(struct sockaddr_in));
	m_address.sin_family = AF_INET;
	inet_pton(AF_INET, host_ip.c_str(), &(m_address.sin_addr));
	m_address.sin_port = htons(m_port);
}

Socket::~Socket()
{
	if (m_good) close(m_socket_fd);
}

void Socket::shutdown()
{
	::shutdown(m_socket_fd, SHUT_RDWR);
}


int Socket::write(const std::string& message, const std::string& hostname,
		unsigned short client_port) const
{
	struct sockaddr_in client_address;
	client_address.sin_family = AF_INET;
	std::string client_ip;

	// Resolve outgoing hostname -- It may fail
	if (hostname_to_ip(hostname, client_ip) != 0) return 0;
	std::cout << "Socket Hostname Resolution: " << hostname << " : " << client_ip << '\n';

	inet_pton(AF_INET, client_ip.c_str(), &(client_address.sin_addr));
	client_address.sin_port = htons(client_port);
	int bytes_sent = sendto(m_socket_fd,
			message.c_str(), message.size(), 0,
			(struct sockaddr*)&client_address,
			sizeof(client_address));
	return bytes_sent;
}

int Socket::read(std::string& message, std::string& client_ip,
		unsigned short& client_port) const
{
	unsigned char buffer[BUFFER_SIZE];
	memset(buffer, 0, BUFFER_SIZE);
	unsigned char client_ip_buf[INET_ADDRSTRLEN + 1];
	struct sockaddr_in client_addr;
	socklen_t client_add_len = sizeof(client_addr);
	int bytes_read = recvfrom(m_socket_fd, buffer, BUFFER_SIZE, 0,
			(sockaddr*)&client_addr, &client_add_len);
	if (bytes_read < 0)
	{
		std::cerr << "Error: failed to completely read the packet\n";
		return bytes_read;
	}
	message = std::string((char*)buffer);
	client_port = ntohs(client_addr.sin_port);
	client_ip_buf[INET_ADDRSTRLEN] = 0;
	inet_ntop(AF_INET,
			&(client_addr.sin_addr),
			(char*)client_ip_buf,
			INET_ADDRSTRLEN);

	if (client_ip_buf == NULL)
		std::cerr << "Error: Failed to get client ip address\n";
	else client_ip = std::string((char*)client_ip_buf);
	return bytes_read;
}
// Only good for IPv4 for now
int Socket::hostname_to_ip(const std::string& hostname, std::string& ip)
{
	struct hostent *he;
	struct in_addr *addr;
	std::cout << "Resolving: " << hostname << '\n';
	if (( he = gethostbyname(hostname.c_str())) == NULL)
	{
		herror("Name Resolution");
		return 1;
	}
	addr = (struct in_addr*) he->h_addr;
	if (addr == NULL) return 1;
	ip = inet_ntoa(*addr);
	return 0;
}


/* UDP Socket */

UDPSocket::UDPSocket(unsigned short port) :
	Socket(port)
{
	setSocketFd();
}

UDPSocket::UDPSocket(std::string host_ip, unsigned short port) :
	Socket(host_ip, port)
{
	setSocketFd();
}

void UDPSocket::setSocketFd()
{
	m_socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (m_socket_fd < 0)
	{
		std::cerr << "Error: Failed to create UDP socket\n";
		m_good = false;
	}
	else m_good = true;

	int socket_opt = 1;
	if (setsockopt(m_socket_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&socket_opt, sizeof(socket_opt)) < 0)
	{
		m_good = false;
		std::cerr << "Error: Failed to set socket options\n";
	}

	// Bind Socket
	if (m_good)
	{
		int bind_value = bind(m_socket_fd, (struct sockaddr*)&m_address,
				sizeof(m_address));
		if (bind_value < 0)
		{
			std::cerr << "Error: Failed to bind socket\n";
			m_bound = false;
		}
		else m_bound = true;
	}
	else m_bound = false;
}

/* SRDP Socket */
/*
SRDPSocket::SRDPSocket(std::string host_ip, unsigned short host_port) :
	UDPSocket(host_ip, host_port)
{ }

int SRDPSocket::write(const std::string& message, const std::string& client_ip,
			unsigned short client_port) const
{

}


int SRDPSocket::read(std::string& message, std::string& client_ip,
			unsigned short& client_port) const
{

}
*/

