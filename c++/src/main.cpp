#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <openssl/sha.h>

#include "Hash.hpp"
#include "ChordKey.hpp"
#include "ChordNode.hpp"
#include "Chord.hpp"

#define TEST_NODES 100

int main()
{
	ChordKey test_key("00:00:00:00:00:00");
	std::cout << "ID: " << test_key.getId() << " : " << test_key.getKey() << '\n';

	Chord maintainer;
	for (unsigned int i = 0; i < TEST_NODES; i++)
	{
		std::ostringstream oss;
		oss << "http://142.104.68.132:" << i;
		maintainer.createNode(oss.str());
	}

	std::cout << TEST_NODES << " created!\n";

	// Node 0 is the boostrapper node
	for (unsigned int i = 1; i < TEST_NODES; i++)
	{
		// Join the nodes together
		std::cout << "Iteration: " << i << '\n';
		ChordNode* node = maintainer.getNode(i);
		std::cout << "Node: " << node << '\n';
		std::cout << maintainer.getNode(0) << '\n';
		node->join(*(maintainer.getNode(0)));
		std::cout << "Joined initial node\n";
		ChordNode* preceding_node = node->getSuccesor()->getPredecessor();

		node->stabilize();

		if (preceding_node == NULL) node->getSuccesor()->stabilize();
		else preceding_node->stabilize();
	}


	return 0;
}

