#ifndef HASH_HPP
#define HASH_HPP


#include <string>
#include <iostream>

#include <string.h>

#include <openssl/sha.h>

/**
 * Hash
 *
 * Simple Openssl SHA1 c++ wrapper
 */
class Hash
{
public:
	Hash(const std::string& input_string);
	const unsigned char operator[](unsigned int i) const;
	inline unsigned int size() const { return 20; }
private:
	unsigned char m_hash[20];
};


std::ostream& operator<<(std::ostream& os, const Hash& h);

#endif//HASH_HPP
