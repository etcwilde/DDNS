#ifndef LOGGING_HPP
#define LOGGING_HPP

#include <iostream>
#include <fstream>
#include <ctime>

#include <time.h>

const std::string currentTime();
const std::string currentDate();

class Log
{
public:
	Log(const std::string& log_file_name)
	{
		m_logfile.open(log_file_name, std::ios::out | std::ios::app);
		m_logfile.rdbuf()->pubsetbuf(0, 0);
	}
	~Log()
	{
		m_logfile.close();
	}

	void write(const std::string& message, char* calling_function = NULL)
	{
		if (calling_function != NULL)
		{
			m_logfile << currentDate() << ' ' << currentTime() <<
				":(" << calling_function << ") "  << message << '\n';

		}
		else
		{
			m_logfile << currentDate() << ' ' << currentTime() <<
				": " << message << '\n';
		}
		m_logfile.flush();
	}
protected:
private:
	std::ofstream m_logfile;
};

#endif//LOGGING_HPP
