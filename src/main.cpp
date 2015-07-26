
/*
 * main
 * File:     main.cpp
 * Author:   Evan Wilde                    <etcwilde@uvic.ca>
 * Date:     Jun 07 2015
 */

#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <cstdlib>

#include "Socket.hpp"
#include "Chord.hpp"
#include "chord_message.pb.h"
#include "HashTable.hpp"
#include "logging.hpp"

#include <sqlite3.h>

#define START_PORT	1025
#define NODES 		1001	// Number of nodes in the system
#define REQUESTS 	50	// Number of random requests made by each node

std::string table_create =
"DROP TABLE IF EXISTS requests;\n"
"\n"
"CREATE TABLE requests\n"
"(\n"
	"req_id integer primary key autoincrement,\n"
	"total_nodes integer,\n"	// All the nodes in the system
	"nid integer,\n"		// Request Sender
	"req_nid integer,\n"		// Who is being requested
	"req_time integer,\n"		// How long it took to make the request
	"miss integer\n"		// Did the request hit a node, or was it a miss?
");\n";

#ifdef DEBUG
int main()
{
	char* sqlite_ErrMsg = 0;

	sqlite3* db;
	int rc = sqlite3_open_v2("results.db", &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
	if (rc)
	{
		std::cerr << "Failed to open database\n";
		sqlite3_close(db);
		return 1;
	}

	std::cout << "Creating Experiment Tables...\n";
	rc = sqlite3_exec(db, table_create.c_str(), NULL, 0, &sqlite_ErrMsg);
	if (rc != SQLITE_OK)
	{
		std::cout << "sqlerr: " << sqlite_ErrMsg << '\n';
		sqlite3_free(sqlite_ErrMsg);
		sqlite3_close(db);
		return 1;
	}
	std::cout << "Tables Created\n";

	GOOGLE_PROTOBUF_VERIFY_VERSION;
	std::vector<DNS::ChordDNS*> nodes;

	DNS::ChordDNS* current_node = new DNS::ChordDNS(std::to_string(0));
	current_node->create(START_PORT);
	nodes.push_back(current_node);
	for (unsigned int n = 1; n < NODES; ++n)
	{
		current_node = new DNS::ChordDNS(std::to_string(n));
		current_node->join("127.0.0.1", START_PORT, START_PORT + n);
		std::cout << "Creating Node: " << n << '\n';
		nodes.push_back(current_node);
		usleep(150 * n); // Give the nodes time to join and stabilize
		if ((nodes.size()) % 5 == 0)
		{
			for (unsigned int i = 0; i < REQUESTS; ++i)
			{
				std::string resolved_ip;
				unsigned short resolved_port;
				unsigned int origin = rand() % nodes.size();
				unsigned int lookup = rand() % (nodes.size() + 2);
				auto begin = std::chrono::high_resolution_clock::now();
				int ret = nodes[origin]->Lookup(std::to_string(lookup),
						resolved_ip, resolved_port);
				auto end = std::chrono::high_resolution_clock::now();
				std::string query =
					"INSERT INTO REQUESTS"
					"(total_nodes, nid, req_nid, req_time, miss) VALUES("
					+ std::to_string(nodes.size()) + ", "
					+ std::to_string(origin) + ", "
					+ std::to_string(lookup) + ", "
					+ std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count())  + ", "
					+ std::to_string(ret) + ");";
				if (sqlite3_exec(db,
						query.c_str(),
						NULL,
						NULL,
						&sqlite_ErrMsg))
				{
					std::cerr << "DB ERROR: " << sqlite_ErrMsg << '\n';
					sqlite3_free(sqlite_ErrMsg);
				}

			}
		}
	}

	sqlite3_close(db);
	std::cout << "Deleting Nodes...\n";
	for (auto x : nodes) delete x;
	std::cout << "NodesDeleted\n";

	std::cout << "\n\tHit Return to exit\n";
	std::string something;
	std::getline(std::cin, something);
	return 0;
}
#else
int main(int argc, const char* argv[])
{
	std::string client_ip;
	unsigned short client_port;
	std::string host_name;
	unsigned short host_port;

	GOOGLE_PROTOBUF_VERIFY_VERSION;
	if (argc != 3 && argc != 5)
	{
		std::cerr<< "Usage: "
			<< argv[0]
			<< " <My Name> <My Port> [<Host IP address> <Host Port Number>]\n";
		return 1;
	}

	host_name = std::string(argv[1]);
	host_port = static_cast<unsigned short>(atoi(argv[2]));
	std::cout << "Host Name: " << host_name << ":" << host_port << '\n';

	DNS::ChordDNS chord_test(host_name);


	// We are alone in the world, we just sit here and be quite
	if (argc == 3)
	{
		//logfile.write("Instancing new chord ring");
		chord_test.create(host_port);
	}
	else // We have the host IP and port number
	{
		//logfile.write("Joining chord ring " + client_ip);
		client_ip = std::string(argv[3]);
		client_port = static_cast<unsigned short>(atoi(argv[4]));
		chord_test.join(client_ip, client_port, host_port);
	}
	std::string dns_name;
	for(;;)
	{
		std::cout << "Lookup: ";
		std::cin >> dns_name;
		if (dns_name.compare("exit") == 0) break;
		else if (dns_name.compare("d") == 0)
		{
			chord_test.d();
			continue;
		}
		else if (dns_name.compare("dump") == 0)
		{
			chord_test.Dump("logs/dump_"+std::to_string(getpid()));
			continue;
		}
		std::string resolved_ip;
		unsigned short resolved_port;
		if(chord_test.Lookup(dns_name, resolved_ip, resolved_port) != 0)
		{
			std::cerr << " Failed To Resolve " << dns_name << '\n';
			continue;
		}
		std::cout << dns_name << " -> " << resolved_ip << ":" << resolved_port << '\n';
	}

	return 0;
}
#endif
