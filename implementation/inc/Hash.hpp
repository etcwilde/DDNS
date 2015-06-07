#ifndef HASH_HPP
#define HASH_HPP

#include <string>
#include <iostream>
#include <sstream>

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
	Hash(const std::string& message)
	{
		memset(m_hash, 0, HASH_LENGTH_BYTES);
		SHA1((const unsigned char*)message.c_str(), message.size(), m_hash);
	}

	inline const unsigned char operator[](unsigned int i) const { return m_hash[i]; }
	inline unsigned long size() const { return HASH_LENGTH_BYTES; }
	std::string toString() const
	{
		std::stringstream os; os << std::hex;
		for (unsigned int i = 0; i < HASH_LENGTH_BYTES; i++)
			os << static_cast<int>(m_hash[i]);
		return os.str();
	}
private:
	unsigned char m_hash[HASH_LENGTH_BYTES];
};

// Nah
// std::ostream& operator<<(std::ostream& os, const Hash& h)
// {
// 	os << std::hex;
// 	for (unsigned int i = 0; i < HASH_LENGTH_BYTES; i++)
// 		os << (int)h[i];
// 	os << std::dec;
// 	return os;
// }
#endif//HASH_HPP
