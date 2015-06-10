
/*
 * Hash
 * File:     Hash.hpp
 * Author:   Evan Wilde                    <etcwilde@uvic.ca>
 * Date:     Jun 02 2015
 */

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
	Hash() { memset(m_hash, 0, HASH_LENGTH_BYTES); }

	Hash(const std::string& message)
	{
		memset(m_hash, 0, HASH_LENGTH_BYTES);
		SHA1((const unsigned char*)message.c_str(), message.size(), m_hash);
	}


	inline void operator=(const Hash& other)
	{ memcpy(m_hash, other.m_hash, HASH_LENGTH_BYTES); }


	int compareTo(const Hash& other) const
	{
		for (unsigned int i = 0; i < HASH_LENGTH_BYTES; i++)
		{
			const int loperand = m_hash[i] & 0xff;
			const int roperand = other.m_hash[i] & 0xff;
			if (loperand != roperand) return (loperand - roperand);
		}
		return 0;
	}

	inline bool operator<(const Hash& other) const
	{ return compareTo(other) < 0; }

	inline bool operator>(const Hash& other) const
	{ return compareTo(other) > 0; }

	inline bool operator==(const Hash& other) const
	{ return compareTo(other) == 0; }

	inline bool operator<= (const Hash& other) const
	{ return compareTo(other) <= 0; }

	inline bool operator>= (const Hash& other) const
	{ return compareTo(other) >= 0; }


	inline const unsigned char operator[](unsigned int i) const { return m_hash[i]; }
	inline unsigned long size() const { return HASH_LENGTH_BYTES; }

	std::string toString() const
	{
		std::stringstream os; os << std::hex;
		for (unsigned int i = 0; i < HASH_LENGTH_BYTES; i++)
			os << static_cast<int>(m_hash[i]);
		return os.str();
	}


	// Hashes the hash for use in hash tables using djb2
	unsigned long djb2() const
	{
		unsigned long hash = 5381;
		for (unsigned long c = 0; c < HASH_LENGTH_BYTES; ++c)
		{
			hash = ((hash << 5) + hash) + m_hash[c];
		}
		return hash;
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
