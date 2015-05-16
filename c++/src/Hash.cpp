#include "Hash.hpp"

Hash::Hash(const std::string& input_string)
{
	memset(m_hash, 0, 20);
	SHA1((const unsigned char*)input_string.c_str(), input_string.size(), m_hash);
}

const unsigned char Hash::operator[](unsigned int i) const
{
	return m_hash[i];
}

std::ostream& operator<<(std::ostream& os, const Hash& h)
{
	os << std::hex;
	for (unsigned int i = 0; i < 20; i++)
		os << (int)h[i];
	os << std::dec;
	return os;
}
