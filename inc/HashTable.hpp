
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
	/**
	 * Insert a value at the key
	 *
	 * If a value is already stored at the key, the value is overwritten
	 */
	virtual void insert(const Hash& key, std::string value) = 0;

	/**
	 * Check if a value is at a key
	 *
	 * Used to determine if a value is stored at a key
	 */
	virtual bool check(const Hash& key, std::string test_value) = 0;

	/**
	 * Returns the value at a key
	 *
	 * Returns the value stored at a key
	 */
	virtual std::string lookup(const Hash& key) = 0;

	/**
	 * Removes a value at a key
	 */
	virtual void remove(const Hash& key) = 0;
};


/**
 * \class LocalHashTable
 *
 * Stores the key value pair locally.
 */
class LocalHashTable : public HashTable
{
public:
	LocalHashTable(unsigned long size=DEFAULT_HASH_TABLE_SIZE) :
		m_size(size), m_table(size, hashFunction) { }

	virtual void insert(const Hash& key, std::string value)
	{ m_table[key] = value; }

	virtual bool check(const Hash& key, std::string test_value)
	{ return (m_table[key].compare(test_value) == 0); }

	virtual std::string lookup(const Hash& h)
	{ return m_table[h]; }

	virtual void remove(const Hash& h)
	{ m_table.erase(h); }

	inline unsigned long size() const { return m_table.size(); }

private:
	static inline size_t hashFunction(const Hash& h)
	{ return h.djb2(); }

	unsigned long m_size;
	std::unordered_map<Hash, std::string, std::function<decltype(hashFunction)> > m_table;
};

/**
 * \class BasicHashTable
 *
 * Uses the value to generate the hash that is used to store the value.
 */
class BasicHashTable : public LocalHashTable
{
public:
	BasicHashTable(unsigned long size=DEFAULT_HASH_TABLE_SIZE) :
	LocalHashTable(size) {}

	virtual void insert(std::string value)
	{  LocalHashTable::insert(Hash(value), value); }

	virtual bool check(std::string value)
	{ return LocalHashTable::check(Hash(value), value); }

	void remove(std::string value)
	{ LocalHashTable::remove(Hash(value)); }

};


class DistributedHashTable : public HashTable
{
public:
	virtual bool check(const Hash& key, std::string test_value)
	{
		std::string ip;
		unsigned short port;
		return check(key, test_value, ip, port);
	}

	/**
	 * Check
	 *
	 * Returns the existence of the value and the ip/port where the value
	 * is stored
	 */
	virtual bool check(const Hash& key, std::string test_value,
			std::string& ip, unsigned short& port)=0;

	virtual std::string lookup(const Hash& key)
	{
		std::string ip; unsigned short port;
		return lookup(key, ip, port);
	}

	/**
	 * Lookup
	 *
	 * Returns the value at a given key and the ip/port where the value is
	 * stored
	 */
	virtual std::string lookup(const Hash& key, std::string& ip,
		       unsigned short& port)=0;

};
#endif//HASHTABLE_HPP
