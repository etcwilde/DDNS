#ifndef CHORDKEY_HPP
#define CHORDKEY_HPP

#include "Hash.hpp"
#include <string>

class ChordKey
{
public:
	ChordKey(const Hash& h);
	ChordKey(const std::string& id);

	/* Comparisons */
	int compareTo(const ChordKey& other) const;
	bool isBetween(const ChordKey& other_1, const ChordKey& other_2) const;
	inline bool operator< (const ChordKey& other) const;
	inline bool operator> (const ChordKey& other) const;
	inline bool operator== (const ChordKey& other) const;
	inline bool operator<= (const ChordKey& other) const;
	inline bool operator>= (const ChordKey& other) const;

	/* Accessors and mutators */
	inline const std::string& getId() const
	{ return m_id; }

	inline void setId(const std::string& id)
	{ m_id = id; }

	inline const Hash& getKey() const
	{ return m_key; }

	inline void setKey(const Hash& h)
	{ m_key = h; }

private:
	Hash m_key;
	std::string m_id;
};
#endif//CHORDKEY_HPP
