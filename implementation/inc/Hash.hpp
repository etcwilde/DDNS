#ifndef HASH_HPP
#define HASH_HPP

#include <string>
#include <iostream>

#include <string.h>
#include <openssl/sha.h>

#define HASH_LENGTH_BITS 160
#define HASH_LENGTH_BYTES 20

/**
 * Hash
 *
 * Simple OpenSSL SHA1 C++ wrapper
 */
class Hash
{
public:
	Hash(const std::string& message);
	inline const unsigned char operator[](unsigned int i) const;
	inline unsigned long size() const { return HASH_LENGTH_BYTES; }
private:
	unsigned char m_hash[HASH_LENGTH_BYTES];
};

std::ostream& operator<<(std::ostream& os, const Hash& h);
#endif//HASH_HPP
