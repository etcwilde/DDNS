#ifndef LOGGING_HPP
#define LOGGING_HPP

#include <iostream>
#include <fstream>
#include <ctime>

#include <time.h>

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




class Log
{
public:
	Log(const std::string& log_file_name)
	{
		m_logfile.open(log_file_name, std::ios::out | std::ios::app);
	}
	~Log()
	{
		m_logfile.close();
	}

	void write(const std::string& message)
	{
		m_logfile << currentDate() << ' ' << currentTime() <<
			": " << message << '\n';
	}
protected:
private:
	std::ofstream m_logfile;
};

#endif//LOGGING_HPP
