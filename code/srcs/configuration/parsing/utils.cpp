#include <netinet/in.h>             // for in_port_t, in_addr_t
#include <stddef.h>                 // for size_t
#include <stdint.h>                 // for uint8_t
#include <cctype>                   // for isdigit, isspace
#include <fstream>                  // for basic_ifstream, basic_ios, basic_...
#include <map>                      // for map, _Rb_tree_iterator, operator!=
#include <string>                   // for basic_string, string, getline
#include <utility>                  // for pair, make_pair
#include <vector>                   // for vector

#include "Route.hpp"                // for Route
#include "ServerConfiguration.hpp"  // for ServerConfiguration
#include "exception.hpp"            // for CustomLineException, CustomException
#include "parsing.hpp"              // for ip_s, ip_t, ipv6_t, ft_hextoint

void	readfile(const char *path, std::string &buff)
{
	std::ifstream	file;

	file.open(path, file.in);
	if (file.fail())
		throw (CustomException("Couldn't open file"));
	std::getline(file, buff, '\0');
	file.close();
}

void	skipLine(std::string &file, size_t &i, size_t &line)
{
	i = file.find('\n', i);
	if (i == std::string::npos)
		throw (CustomLineException("Unexpected error", line));
	if (i != file.size())
		line++;
	i++;
}

void	skipWSpace(std::string &file, size_t &i, size_t &line)
{
	if (i == std::string::npos)
		throw (CustomLineException("Unexpected error", line));
	while (std::isspace(static_cast<unsigned char>(file[i])))
	{
		if (file[i] == '\n')
			line++;
		i++;
	}
}

short	realAtoi(std::string &file, size_t &i, size_t &line, short max, short len)
{
	short	nb = 0;

	while (len-- > 0 && std::isdigit(file[i]))
	{
		nb = nb * 10 + (file[i] - '0');
		i++;
		len++;
	}
	if (nb > max || nb < 0)
		throw (CustomLineException("Wrong number format", line));
	return (nb);
}

uint8_t	hexToInt(std::string &file, size_t &i, size_t &line)
{
	int		j = 2;
	short	nb = 0;
	const std::string	hex = "0123456789abcdef";

	while (file[i] && hex.find(file[i]) != std::string::npos && j > 0)
	{
		nb = nb * 16 + hex.find(file[i]);
		i++;
		j--;
	}
	if (j != 0)
		throw (CustomLineException("Wrong IP format", line));
	return (nb);
}

void	insertHost(std::map<ip_t, std::vector<ServerConfiguration> > &conf, std::vector<std::string> \
&serverNames, std::map<unsigned short, std::string> &errorPages, size_t &maxClientBodySize, \
std::map<std::string, Route> &routes, std::string &root, ip_t &ip, std::vector<std::string> &index)
{
	for (std::map<ip_t, std::vector<ServerConfiguration> >::iterator it = conf.begin(); it != conf.end(); ++it)
	{
		if (!it->first.ipv4.empty())
		{
			for (std::map<in_addr_t, in_port_t>::iterator itt = ip.ipv4.begin(); itt != ip.ipv4.end();)
			{
				if (it->first.ipv4.begin()->first == itt->first && it->first.ipv4.begin()->second == itt->second)
				{
					it->second.push_back(ServerConfiguration(serverNames, errorPages, maxClientBodySize, routes, root, index));
					std::map<in_addr_t, in_port_t>::iterator temp = itt;
					++itt;
					ip.ipv4.erase(temp);
				}
				else
					++itt;
			}
		}
		else if (!it->first.ipv6.empty())
		{
			for (std::map<ipv6_t, in_port_t>::iterator itt = ip.ipv6.begin(); itt != ip.ipv6.end();)
			{
				if (it->first.ipv6.begin()->first == itt->first && it->first.ipv6.begin()->second == itt->second)
				{
					it->second.push_back(ServerConfiguration(serverNames, errorPages, maxClientBodySize, routes, root, index));
					std::map<ipv6_t, in_port_t>::iterator temp = itt;
					++itt;
					ip.ipv6.erase(temp);
				}
				else
					++itt;
			}
		}
		else if (!it->first.unix_adrr.empty())
		{
			for (std::vector<std::string>::iterator itt = ip.unix_adrr.begin(); itt != ip.unix_adrr.end(); ++itt)
			{
				if (it->first.unix_adrr[0] == *itt)
				{
					it->second.push_back(ServerConfiguration(serverNames, errorPages, maxClientBodySize, routes, root, index));
					std::vector<std::string>::iterator temp = itt;
					++itt;
					ip.unix_adrr.erase(temp);
				}
				else
					++itt;
			}
		}
	}
	for (std::map<in_addr_t, in_port_t>::iterator it = ip.ipv4.begin(); it != ip.ipv4.end(); ++it)
	{
		ip_t	temp;
		std::vector<ServerConfiguration> serv;

		serv.push_back(ServerConfiguration(serverNames, errorPages, maxClientBodySize, routes, root, index));
		temp.ipv4.insert(*it);
		conf.insert(std::make_pair(temp, serv));
	}
	for (std::map<ipv6_t, in_port_t>::iterator it = ip.ipv6.begin(); it != ip.ipv6.end(); ++it)
	{
		ip_t	temp;
		std::vector<ServerConfiguration> serv;

		serv.push_back(ServerConfiguration(serverNames, errorPages, maxClientBodySize, routes, root, index));
		temp.ipv6.insert(*it);
		conf.insert(std::make_pair(temp, serv));
	}
	for (std::vector<std::string>::iterator it = ip.unix_adrr.begin(); it != ip.unix_adrr.end(); ++it)
	{
		ip_t	temp;
		std::vector<ServerConfiguration> serv;

		serv.push_back(ServerConfiguration(serverNames, errorPages, maxClientBodySize, routes, root, index));
		temp.unix_adrr.push_back(*it);
		conf.insert(std::make_pair(temp, serv));
	}
	for (std::map<ip_t, std::vector<ServerConfiguration> >::iterator it = conf.begin(); it != conf.end(); ++it)
	{
		for (std::vector<ServerConfiguration>::iterator itt = it->second.begin(); itt != it->second.end(); ++itt)
		{
			for (std::map<std::string, Route>::const_iterator ittt = itt->getRoutes().begin(); ittt != itt->getRoutes().end(); ++ittt)
			{
				Route& route = const_cast<Route&>(ittt->second);
    			if (route.getIndex().empty())
        			route.setIndex(itt->getIndex());
			}
		}
	}
}
