#ifndef CHORDKEY_HPP
#define CHORDKEY_HPP

#include "Hash.hpp"
#include <string>
#include <ostream>

class ChordKey
{
public:
	ChordKey(const Hash& h);
	ChordKey(const std::string& id);

	/* Comparisons */
	int compareTo(const ChordKey& other) const;
	bool isBetween(const ChordKey& other_1, const ChordKey& other_2) const;
	inline bool operator< (const ChordKey& other) const
	{ return compareTo(other) < 0; }

	inline bool operator> (const ChordKey& other) const
	{ return compareTo(other) > 0; }

	inline bool operator== (const ChordKey& other) const
	{ return compareTo(other) == 0; }

	inline bool operator<= (const ChordKey& other) const
	{ return compareTo(other) <= 0; }

	inline bool operator>= (const ChordKey& other) const
	{ return compareTo(other) >= 0; }
	/* Accessors and mutators */
	inline const std::string& getId() const
	{ return m_id; }

	inline void setId(const std::string& id)
	{ m_id = id; }

	inline void setIdHash(const std::string& id)
	{ m_id = id; m_key = Hash(id); }

	inline const Hash& getKey() const
	{ return m_key; }

	inline void setKey(const Hash& h)
	{ m_key = h; }

	friend std::ostream& operator<<(std::ostream& os, const ChordKey& k);


private:
	Hash m_key;
	std::string m_id;
};

#endif//CHORDKEY_HPP
