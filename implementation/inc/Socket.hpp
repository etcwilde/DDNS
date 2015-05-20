#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <arpa/inet.h>
#include <assert.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>

#include <string>
#include <iostream>


#define BUFFER_SIZE 1024

class Socket
{
public:
	~Socket();

	int write(const std::string& message, const std::string& client_ip,
			unsigned short client_port);

	int read(std::string& message, std::string& client_ip,
			unsigned short& client_port);

protected:
	Socket(unsigned int port);

	Socket(std::string host_ip, unsigned int port);

	inline void setSocketFd(unsigned int socket_fd);

private:

	bool m_bound;
	bool m_good;

	unsigned int m_port;
	struct sockaddr_in m_address;

	unsigned int m_socket_fd;
};


#endif//SOCKET_HPP
