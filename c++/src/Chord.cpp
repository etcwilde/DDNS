#include "Chord.hpp"

Chord::Chord() { }

Chord::~Chord()
{
	for (ChordNode* n : m_nodelist) delete n;
}

void Chord::createNode(std::string id)
{
	ChordNode* node = new ChordNode(id);
	m_nodelist.push_back(node);

	if (m_keymap[node->getKey()] != NULL)
	{
		std::cerr << "Error: " << node->getKey()
			<< " already exists!\n";
		m_nodelist.pop_back();
		delete node;
	}
	else m_keymap[node->getKey()] = node;
}

ChordNode* Chord::getNode(unsigned int i)
{
	return m_nodelist[i];
}

/*ChordNode* Chord::getNode(const ChordKey& k)
{
	return m_nodelist[m_keymap[k]];
}
*/

