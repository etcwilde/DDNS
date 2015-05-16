#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <openssl/sha.h>

#include "Hash.hpp"
#include "ChordKey.hpp"
#include "ChordNode.hpp"


int main()
{
	//Hash test("Hello World");
	//std::cout << test << '\n';

	ChordKey test_key("00:00:00:00:00:00");
	std::cout << "ID: " << test_key.getId() << " : " << test_key.getKey() << '\n';

	return 0;
}

