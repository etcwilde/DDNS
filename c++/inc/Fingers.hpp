#ifndef FINGERS_HPP
#define FINGERS_HPP

#include <vector>

#include "ChordKey.hpp"
#include "ChordNode.hpp"
#include "Hash.hpp"

class Finger
{
public:
	Finger(ChordKey start_key, ChordNode* node);

	inline ChordKey getKey() const { return m_start_key; }
	inline void setStart(ChordKey start) { m_start_key = start; }
	inline ChordNode* getNode() { return m_node; }
	inline void setNode(ChordNode* node) { m_node = node; }


private:
	ChordKey m_start_key;
	ChordNode* m_node;
};

class FingerTable
{
public:
	FingerTable(ChordNode* node);

	inline Finger getFinger(unsigned int i) { return m_fingers[i]; }
private:
	std::vector<Finger> m_fingers;
};

#endif//FINGERS_HPP
