#ifndef CHORD_HPP
#define CHORD_HPP

#include <string>
#include <mutex>
#include <thread>

#include <unistd.h>

#include "ChordKey.hpp"

#define DEFAULT_CHORD_PORT 1994 // Default connection port if no port supplied
#define DEFAULT_HEART_BEAT 1000 // Ping every 1 seconds
#define DEFAULT_RESILLIANCY 3 // Three pulse misses and it is dead


// We use a heartbeat to determine if the node is still alive. When three
// pulses are missed, the node is dead and should be removed from the chord
// ring

// WHAT HAVE I DONE THIS ISN'T SUPPOSED TO BE DEPRESSING...

// We store the information about or neighbors
typedef struct
{
	double heartbeat;
	unsigned int resiliancy;
} neighbor_stats_t;


template<class T>
class Chord
{
public:
	Chord() :
		m_heartbeat(DEFAULT_HEART_BEAT),
		m_resiliancy(DEFAULT_RESILLIANCY),
		m_dead(false)
	{ m_heart = std::thread(&Chord::heartBeat, this); }

	~Chord()
	{
		m_dead = true;
		m_heart.join();
		//It will be at most as long as the heartbeat duration
	}
	// Local things
	void start();
	bool join(std::string group_ip, unsigned int group_port);
	bool join(std::string group_ip);
	void disconnect();
	void update(T new_information) const; // Sends update message

	// Local
	inline T getValue() const { return m_value; }
	inline void setValue(T value) const { m_value = value; }


	// Global
	T lookup(ChordKey key);

protected:
	virtual void pulse(); // Function called with each heart beat

private:
	// Network stuff? I don't know yet

private:
	// Heart stuff
	void heartBeat(); // Calls the pulse function and handles the thread
	double m_heartbeat; // Delay time of the heart (milliseconds)
	unsigned int m_resiliancy; // How long we wait before saying a node is dead
	bool m_dead;
	std::thread m_heart;
	neighbor_stats_t m_neighbors[2];

private:
	// value
	T m_value;
};

template <class T>
void Chord<T>::pulse()
{
	std::cout << " ** thump ** \n";
}

template <class T>
void Chord<T>::heartBeat()
{
	while (!m_dead)
	{
		pulse();
		usleep(m_heartbeat * 1000);
	}
}




#endif//CHORD_HPP
