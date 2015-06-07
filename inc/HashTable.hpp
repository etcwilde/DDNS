
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
	m_table(size) { }

	// Note: This will over-wite anything in there
	virtual void insert(std::string item)
	{ m_table[Hash(item).djb2() % m_size] = item; }

	// No error checking
	virtual bool check(std::string item)
	{ return m_table[Hash(item).djb2() % m_size].compare(item) == 0; }

	// No checking done
	virtual std::string lookup(const Hash& h) const
	{ return m_table[h.djb2() % m_size]; }

	inline unsigned long size() const { return m_size; }

private:
	unsigned long m_size;
	std::vector<std::string> m_table;
};


#endif//HASHTABLE_HPP
