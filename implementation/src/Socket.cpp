#include "Socket.hpp"




Socket::Socket(unsigned int port) :
	m_bound(false),
	m_good(false),
	m_port(port)
{
	bzero((char*)&m_address, sizeof(struct sockaddr_in));
	m_address.sin_family = AF_INET;
	m_address.sin_addr.s_addr = INADDR_ANY;
	m_address.sin_port = htons(m_port);
}

Socket::Socket(std::string host_ip, unsigned int port) :
	m_bound(false),
	m_good(false),
	m_port(port)
{
	bzero((char*)&m_address, sizeof(struct sockaddr_in));
	m_address.sin_family = AF_INET;
	inet_pton(AF_INET, host_ip.c_str(), &(m_address.sin_addr));
	m_address.sin_port = htons(m_port);
}


void Socket::setSocketFd(unsigned int socket_fd)
{
	m_socket_fd = socket_fd;
}


int Socket::write(const std::string& message, const std::string& clientname,
		unsigned short client_port)
{
	struct sockaddr_in client_address;
	client_address.sin_family = AF_INET;
	inet_pton(AF_INET, clientname.c_str(), &(client_address.sin_addr));
	client_address.sin_port = htons(client_port);
}

int Socket::read(std::string& message, std::string& client_address,
		unsigned short& client_port)
{
	unsigned char buffer[BUFFER_SIZE];
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
}

