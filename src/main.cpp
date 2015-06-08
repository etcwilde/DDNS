
/*
 * main
 * File:     main.cpp
 * Author:   Evan Wilde                    <etcwilde@uvic.ca>
 * Date:     Jun 07 2015
 */

#include <iostream>
#include <string>

#include "Socket.hpp"
#include "ChordKey.hpp"
#include "Chord.hpp"
#include "chord_message.pb.h"
#include "HashTable.hpp"

#define DEFAULT_PORT 8080

int main(int argc, const char* argv[])
{
	/*std::string test_message("Hello World");
	HashTable table;
	Hash h(test_message);
	table.insert(test_message);
	std::cout << table.check("Goodbye") << '\n';
	std::cout << table.check(test_message) << '\n';
	std::cout << table.check("Helloworld") << '\n';
	std::cout << table.lookup(test_message) << '\n'; */


	//std::string test_message("Hello World");
	BasicHashTable table;
	std::string test_string("Hello World");
	table.insert(test_string);
	std::cout << table.check(test_string) << '\n';
	std::cout << table.check("Another string") << '\n';
	std::cout << table.lookup(Hash(test_string)) << '\n';
	table.insert("Another string");
	std::cout << table.check("Another string") << '\n';






	GOOGLE_PROTOBUF_VERIFY_VERSION;
	UDPSocket* socket;
	if (argc != 2 && argc != 4)
	{
		std::cout << "Usage: " << argv[0] << " <My Port> <Host IP address> <Host Port Number>\n";
		return 1;
	}

	unsigned short host_port = (unsigned short)atoi(argv[1]);
	Chord::Request request;
	std::string message;
	std::string client_ip;
	unsigned short client_port;
	Hash hash(std::to_string(host_port));
	std::cout << "Id: " << hash.toString() << '\n';
	std::cout << "Hash djb2: " << hash.djb2() << '\n';

	// We are alone in the world, we just sit here and be quite
	if (argc == 2)
	{
		// We are alone and no chord
		socket = new UDPSocket(host_port);

		socket->read(message, client_ip, client_port);
		request.ParseFromString(message);
		switch (request.type())
		{
			case Chord::Request::JOIN:
				std::cout << "JOIN request From: "
					<< client_ip << ":" << client_port
					<< "| " << request.content() << " :\n"
					<< request.id() << '\n';
				break;

			case Chord::Request::DROP:
				std::cout << "Leave request From: "
					<< client_ip << ":" << client_port
					<< "| " << request.content() << " :\n"
					<< request.id() << '\n';
				break;
			case Chord::Request::GET:
				std::cout << "GET request From: "
					<< client_ip << ":" << client_port
					<< "| " << request.content() << " :\n"
					<< request.id() << '\n';
				break;

			case Chord::Request::SET:
				std::cout << "SET request From: "
					<< client_ip << ":" << client_port
					<< "| " << request.content() << " :\n"
					<< request.id() << '\n';
				break;

			default:
				std::cout << "You suck!\n";
		}

		request.set_id(hash.toString());
		request.set_type(Chord::Request::DROP);
		request.set_content("Goodbye, it was fun!");
		request.SerializeToString(&message);
		socket->write(message, client_ip, client_port);
	}
	else
	{
		client_ip = std::string(argv[2]);
		client_port = (unsigned short)atoi(argv[3]);
		// We are not alone
		socket = new UDPSocket(host_port);

		request.set_id(hash.toString());
		request.set_type(Chord::Request::SET);
		request.set_content("Hello from mars");

		request.SerializeToString(&message);
		socket->write(message, client_ip, client_port);

		socket->read(message, client_ip, client_port);
		request.ParseFromString(message);
		switch (request.type())
		{
			case Chord::Request::JOIN:
				std::cout << "JOIN request From: "
					<< client_ip << ":" << client_port
					<< "| " << request.content() << " :\n"
					<< request.id() << '\n';
				break;

			case Chord::Request::DROP:
				std::cout << "Leave request From: "
					<< client_ip << ":" << client_port
					<< "| " << request.content() << " :\n"
					<< request.id() << '\n';
				break;
			case Chord::Request::GET:
				std::cout << "GET request From: "
					<< client_ip << ":" << client_port
					<< "| " << request.content() << " :\n"
					<< request.id() << '\n';
				break;

			case Chord::Request::SET:
				std::cout << "SET request From: "
					<< client_ip << ":" << client_port
					<< "| " << request.content() << " :\n"
					<< request.id() << '\n';
				break;

			default:
				std::cout << "You suck!\n";
		}

	}
	delete socket;
	return 0;
}
