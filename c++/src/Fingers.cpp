#include "Fingers.hpp"

Finger::Finger(ChordKey start, ChordNode* node) :
	m_start_key(start),
	m_node(node)
{ }

FingerTable::FingerTable(ChordNode* node) :
	m_fingers(HASH_LENGTH_BITS)
{
	for (unsigned int i = 0; i < m_fingers.size(); i++)
	{
		ChordKey start = node->getKey();
	}
}
