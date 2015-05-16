#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <openssl/sha.h>

#include "Hash.hpp"


int main()
{
	Hash test("Hello World");
	std::cout << test << '\n';

	return 0;
}

