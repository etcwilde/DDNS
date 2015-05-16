#ifndef CHORDNODE_HPP
#define CHORDNODE_HPP

// Algorithm on Wikipedia

#include <string>
#include <vector>

#include "ChordKey.hpp"


class ChordNode
{
public:
	ChordNode(const std::string& id);

	const ChordNode* findSuccessor(const std::string& id) const;

	/**
	 * \brief Create a new chord ring
	 *
	 * This is used for initialization purposes
	 */
	void create();

	/**
	 * \brief Joins this chord ring another chord ring
	 *
	 * \param n A boot-strapping node
	 *
	 */
	void join(const ChordNode& n);

	void notify(const ChordNode& n);

	/**
	 * \brief Verifies successor and predecessor
	 *
	 * Call periodically
	 */
	void stabilize();

	void notify(ChordNode* node);

	void fixFingers();

	void checkPredecessor();

	inline const ChordKey& getKey() const { return m_key; }
	inline const std::string getId() const { return m_id; }

	inline ChordNode* getPredecessor() { return m_predecessor; }
	inline ChordNode* getSuccesor() { return m_successor; }

protected:

	ChordNode();

private:

private:
	std::string m_id;
	ChordKey m_key;
	ChordNode* m_predecessor;
	ChordNode* m_successor;
};

#endif//CHORDNODE_HPP
