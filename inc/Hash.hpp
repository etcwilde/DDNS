
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
#include <stdio.h>
#include <openssl/sha.h>

//#define HASH_LENGTH_BITS 160
//#define HASH_LENGTH_BYTES 20
#define HASH_LENGTH_BYTES 20
#define HASH_LENGTH_BITS HASH_LENGTH_BYTES * 8

/**
 * Hash
 *
 * Simple OpenSSL SHA1 C++ wrapper
 */
class Hash
{
	public:
		Hash() { memset(m_hash, 0, HASH_LENGTH_BYTES); }

		Hash(const std::string& message, bool hashed=false)
		{
			if (!hashed)
			{
				memset(m_hash, 0, HASH_LENGTH_BYTES);
				SHA1((const unsigned char*)message.c_str(), message.size(), m_hash);
			}
			else memcpy(m_hash, message.c_str(), HASH_LENGTH_BYTES);
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

		bool between(const Hash& smaller, const Hash& larger)
		{
			int small_large = smaller.compareTo(larger);
			if (small_large < 0)
			{
				if (compareTo(smaller) > 0 && compareTo(larger) < 0)
					return true;
			}
			else if (small_large > 0) // small and large are reversed
			{
				if (compareTo(larger) < 0 || compareTo(smaller) > 0)
					return true;
			}
			return false;
		}

		inline bool operator<(const Hash& other) const
		{ return compareTo(other) < 0; }

		inline bool operator>(const Hash& other) const
		{ return compareTo(other) > 0; }

		inline bool operator==(const Hash& other) const
		{ return compareTo(other) == 0; }

		inline bool operator!=(const Hash& other) const
		{ return compareTo(other) != 0; }

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

		std::string raw() const
		{
			return std::string(reinterpret_cast<const char*>(&m_hash[0]),
					HASH_LENGTH_BYTES);
		}


		// Hashes the hash for use in hash tables using djb2
		unsigned long djb2() const
		{
			unsigned long hash = 5381;
			for (unsigned long c = 0; c < HASH_LENGTH_BYTES; ++c)
				hash = ((hash << 5) + hash) + m_hash[c];
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
