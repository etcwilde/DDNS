#include <iostream>
#include <string>

#include "Socket.hpp"
#include "ChordKey.hpp"

int main()
{
	ChordKey cumulus_key("Cumulus");
	ChordKey storm_key("Stormageddon");
	ChordKey strat_key("Stratus");
	std::cout << "Cumulus Key: " << cumulus_key << '\n';
	std::cout << "Storm Key: " << storm_key << '\n';
	std::cout << "Stratus Key: " << strat_key << '\n';
	std::cout << "Compare storm strat: " << storm_key.compareTo(strat_key) << '\n';
	std::cout << "Compare strat storm: " << strat_key.compareTo(storm_key) << '\n';
	std::cout << "Compare strat start: " << strat_key.compareTo(strat_key) << '\n';
	std::cout << "Between: " << storm_key.isBetween(strat_key, cumulus_key) << '\n';
	return 0;
}
