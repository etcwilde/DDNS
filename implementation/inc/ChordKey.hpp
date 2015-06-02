#ifndef CHORDKEY_HPP
#define CHORDKEY_HPP

#include "Hash.hpp"
#include <string>
#include <ostream>

class ChordKey
{
public:
	ChordKey(const std::string& id) : m_key(Hash(id)), m_id(id) { }

	// Comparisons

	/**
	 * \brief Compare To
	 *
	 * Compares two chord keys
	 *
	 * Return:
	 *
	 * if left > right: >0
	 * if left = right: 0
	 * if left < right: <0
	 */
	int compareTo(const ChordKey& other) const
	{
		for (unsigned int i = 0; i < m_key.size(); i++)
		{
			const int loperand = m_key[i] & 0xff;
			const int roperand = other.m_key[i] & 0xff;
			if (loperand != roperand) return (loperand - roperand);
		}
		return 0;
	}

	inline bool isBetween(const ChordKey& from, const ChordKey& to) const
	{
		const int from_to_compare = from.compareTo(to);
		if (from_to_compare < 0) if (*this > from && *this < to) return true;
		else if (from_to_compare > 0) if (*this < to || *this > from) return true;
		return false;
	}

	inline bool operator<(const ChordKey& other) const
	{ return compareTo(other) < 0; }

	inline bool operator>(const ChordKey& other) const
	{ return compareTo(other) > 0; }

	inline bool operator==(const ChordKey& other) const
	{ return compareTo(other) == 0; }

	inline bool operator<= (const ChordKey& other) const
	{ return compareTo(other) <= 0; }

	inline bool operator>= (const ChordKey& other) const
	{ return compareTo(other) >= 0; }

	// Accessors and mutators
	inline const std::string& getId() const
	{ return m_id; }

	/**
	 * Updates hash and id
	 */
	inline void setId(std::string& id)
	{ m_id = id; m_key = Hash(id); }

	inline const Hash& getKey() const
	{ return m_key; }

	// I dunno about this one
	inline void setKey(const Hash& h)
	{ m_key = h; }

	friend std::ostream& operator<<(std::ostream& os, const ChordKey& k);

private:
	Hash m_key;
	std::string m_id;
};

std::ostream& operator<<(std::ostream& os, const ChordKey& k)
{
	os << k.getKey();
	return os;
}

#endif//CHORDKEY_HPP
