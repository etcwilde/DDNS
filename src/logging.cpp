#include "logging.hpp"
const std::string currentTime()
{
	time_t current = time(0);
	char buf[80];
	strftime(buf, sizeof(buf), "%H:%M:%S %P", localtime(&current));
	return buf;
}

const std::string currentDate()
{
	time_t current = time(0);
	char buf[80];
	strftime(buf, sizeof(buf), "%d/%m/%Y", localtime(&current));
	return buf;
}
