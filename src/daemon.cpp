#include "daemon.hpp"

using namespace Chord;

//#define HOSTNAME_PATTERN "^hostname[ \t]*?=[ \t]*?([0-9a-zA-Z\\.]+?)[ \t]*?(?:#.*)?$"
//#define HOSTFILE_PATTERN "^hostfile[ \t]*?=[ \t]*?([0-9a-zA-Z\\/.]+?)[ \t]*?(?:#.*)?$"
//#define SOCKET_PATTERN "^socketfile[ \t]*?=[ \t]*?([0-9a-zA-Z\\/.]+?)[ \t]*?(?:#.*)?$"
#define HOSTS_PATTERN "^((?:[0-9]{1,3}[.]){3}[0-9]{1,3})[ \t]+([a-zA-Z0-9._-]+)(?:[ \t]+([a-zA-Z0-9_-]+)?)?[ \t]?#?.*$"

daemon::daemon(const std::string& hostname,
		const std::string& sockfile,
		const unsigned int& port,
		const std::string& hostfname):
	m_hostname(hostname),
	m_sockfilename(sockfile),
	m_port(port),
	m_hostfilename(hostfname)
{
	//m_hosts
	// Read hostfile
	if (hostfname.compare("") == 0) { return; }
	std::fstream hostfile(hostfname);
	std::smatch groups;
	std::string line;
	if (hostfile.is_open())
	{
		while(getline(hostfile, line))
		{
			if (line.c_str()[0] == '#') continue;
			if(std::regex_match(line, groups, std::regex(HOSTS_PATTERN)))
			{
				// IP: 		groups[1]
				// Name: 	groups[2/3]
				// Position:	hostfile.tellg();
				if(groups.length(3) == 0) m_hosts.push_back(
						std::tuple<std::string, std::string, unsigned int>(
							groups[1], groups[2],
							hostfile.tellg()));
				else m_hosts.push_back(
						std::tuple<std::string, std::string, unsigned int>(
							groups[1], groups[3],
							hostfile.tellg()));
			}

		}
	}
	else
	{
		std::cout << "hostfile does not exist\n";
	}
	hostfile.close();
}


const std::vector<std::tuple< std::string, std::string, unsigned int> >& daemon::hosts()const
{
	return m_hosts;
}


