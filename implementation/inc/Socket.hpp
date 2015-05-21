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
#include <unistd.h>

#include <string>
#include <iostream>


#define BUFFER_SIZE 1024

class Socket
{
public:
	~Socket();

	int write(const std::string& message, const std::string& client_ip,
			unsigned short client_port) const;

	int read(std::string& message, std::string& client_ip,
			unsigned short& client_port) const;

protected:
	Socket(unsigned int port);

	Socket(std::string host_ip, unsigned int port);

	// Performs the binding and checking of the socket
	virtual void setSocketFd() = 0;

	bool m_bound;
	bool m_good;

	unsigned int m_port;
	struct sockaddr_in m_address;

	unsigned int m_socket_fd;

private:


};


/**
 * Unreliable Datagram Socket
 */
class UDPSocket : public Socket
{
public:
	UDPSocket(unsigned int port);
	UDPSocket(std::string host_ip, unsigned int port);

protected:
private:
	virtual void setSocketFd();

};

class RDSocket : public Socket
{
public:
	RDSocket(unsigned int port);
	RDSocket(std::string host_ip, unsigned int port);
protected:
private:
	virtual void setSocketFd();
};

#endif//SOCKET_HPP
