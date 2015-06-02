#include "Hash.hpp"

Hash::Hash(const std::string& message)
{
	memset(m_hash, 0, HASH_LENGTH_BYTES);
	SHA1((const unsigned char*)message.c_str(), message.size(), m_hash);
}

inline const unsigned char Hash::operator[](unsigned int i) const
{
	return m_hash[i];
}

std::ostream& operator<<(std::ostream& os, const Hash& h)
{
	os << std::hex;
	for (unsigned int i = 0; i < HASH_LENGTH_BYTES; i++)
		os << (int)h[i];
	os << std::dec;
	return os;
}
