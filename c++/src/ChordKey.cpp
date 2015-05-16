#include "ChordKey.hpp"

ChordKey::ChordKey(const Hash& h) :
	m_key(h)
{ }

ChordKey::ChordKey(const std::string& id) :
	m_id(id),
	m_key(Hash(id))
{ }

int ChordKey::compareTo(const ChordKey& other) const
{
	for (unsigned int i = 0; i < m_key.size(); i++)
	{
		const int loperand = m_key[i] & 0xff;
		const int roperand = other.m_key[i] & 0xff;
		if (loperand != roperand) return (loperand - roperand);
	}
	return 0;
}

bool ChordKey::isBetween(const ChordKey& o1, const ChordKey& o2) const
{
	const int from_to_compare = o1.compareTo(o2);
	if (from_to_compare < 0) if (*this > o1 && *this < o2) return true;
	else if (from_to_compare > 0) if (*this < o2 || *this > o1) return true;
	return false;
}

