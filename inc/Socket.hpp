
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

	virtual int write(const std::string& message, const std::string& client_ip,
			unsigned short client_port) const;

	virtual int read(std::string& message, std::string& client_ip,
			unsigned short& client_port) const;

	void shutdown();

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


/**
 * Somewhat Reliable Datagram Protocol
 *
 * We want it to be able to ensure that a packet makes it
 * We don't care about ordering
 *
 * Not Implemented
 *
 *
 */
/*
class SRDPSocket : public UDPSocket
{
public:
	SRDPSocket(std::string host_ip, unsigned short port);

	virtual int write(const std::string& message,
			const std::string& client_ip,
			unsigned short client_port) const;

	virtual int read(std::string& message, std::string& client_ip,
			unsigned short& client_port) const;

protected:
	typedef struct
	{
		unsigned short seqno;
		unsigned short ackno;

		unsigned short flags; // Don't know what flags yet
	} header_t;

	// Current status
	unsigned short m_seqno;
	unsigned short m_ackno;
};
*/

#endif//SOCKET_HPP
