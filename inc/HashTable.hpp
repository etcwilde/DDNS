
/*
 * HashTable
 * File:     HashTable.hpp
 * Author:   Evan Wilde                    <etcwilde@uvic.ca>
 * Date:     Jun 07 2015
 */

#ifndef HASHTABLE_HPP
#define HASHTABLE_HPP

#include <utility>
#include <vector>
#include <unordered_map>

#include "Hash.hpp"


#define DEFAULT_HASH_TABLE_SIZE	512 // 512 items

/**
 * A datastructure to store and lookup data
 *
 * This is to outline the requirements for the distributed hash table
 * Which will be used to implement Chord, which uses SHA1 hashing algorithm,
 * which does not work with the standard c++ unordered_map hasher as it is too
 * big.
 *
 * We use the djb hasher on the hashes to get them small enough to fit into our
 * maps. We may want to switch at some point to use c++ map though with this
 * bonus feature. Then we don't need to define a table size, c++ will handle
 * that for us.
 */

class HashTable
{
public:
	HashTable(unsigned long size=DEFAULT_HASH_TABLE_SIZE) : m_size(size),
	m_table(size, hashFunction) { }

	// Note: This will over-wite anything in there
	virtual void insert(std::string item)
	{ m_table[Hash(item)] = item; }

	// No error checking
	virtual bool check(std::string item)
	{ return m_table[Hash(item)].compare(item) == 0; }

	// No checking done
	virtual std::string lookup(const Hash& h)
	{ return m_table[h]; }

	virtual void remove(const Hash& h)
	{ m_table.erase(h); }

	virtual void remove(const std::string& s)
	{ m_table.erase(Hash(s)); }

	inline unsigned long size() const { return m_table.size(); }

private:
	static inline size_t hashFunction(const Hash& h)
	{
		return h.djb2();
	}

	std::unordered_map<Hash, std::string, std::function<decltype(hashFunction)> > m_table;
	unsigned long m_size;
};


#endif//HASHTABLE_HPP
