#ifndef CHORD_HPP
#define CHORD_HPP

#include <string>
#include <vector>
#include <map>

#include "ChordKey.hpp"
#include "ChordNode.hpp"
#include "Hash.hpp"

class Chord
{
public:
	Chord(); // Not sure
	~Chord(); // yeah
	void createNode(std::string id);
	ChordNode* getNode(unsigned int i);
	ChordNode* getNode(const ChordKey& k);

protected:
private:
	std::vector<ChordNode*> m_nodelist;
	std::map<ChordKey, ChordNode*> m_keymap;
};

#endif//CHORD_HPP
