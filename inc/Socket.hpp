
/*
 * Socket
 * File:     Socket.hpp
 * Author:   Evan Wilde                    <etcwilde@uvic.ca>
 * Date:     May 20 2015
 */

#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <time.h>
#include <unistd.h>

#include <string>
#include <iostream>

#define BUFFER_SIZE 1024



class Socket
{
public:

	virtual ~Socket();

	virtual int write(const std::string& message,
			const std::string& client_ip,
			unsigned short client_port) const;

	virtual int read(std::string& message,
			std::string& client_ip,
			unsigned short& client_port) const;
	void shutdown();
	static int hostname_to_ip(const std::string& hostname,
			std::string& ip);

protected:
	Socket(unsigned short port);

	Socket(std::string host_ip, unsigned short port);

	// Performs the binding and checking of the socket
	virtual void setSocketFd() = 0;

	bool m_bound;
	bool m_good;

	unsigned short m_port;
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
	UDPSocket(unsigned short port);
	UDPSocket(std::string host_ip, unsigned short port);

protected:
private:
	virtual void setSocketFd();
};


// Stream Socket

/**
 * Unix Stream Socket
 */

class UnixSocket
{
public:
	UnixSocket(const std::string& socket_name);
	int Listen(); // Used for server
	int Connect(const std::string& socket_name); // Used for client

private:
	void setSocketFd();

	struct sockaddr_un m_address;
	bool m_good;
	bool m_bound;
	unsigned int m_socket_fd;
};



#endif//SOCKET_HPP
