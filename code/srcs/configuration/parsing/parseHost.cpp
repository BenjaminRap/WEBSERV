#include <netinet/in.h>   // for in_port_t, in_addr_t
#include <stddef.h>       // for size_t
#include <stdint.h>       // for uint8_t, uint32_t
#include <sys/types.h>    // for ssize_t
#include <cctype>         // for isdigit, isxdigit
#include <map>            // for map, _Rb_tree_const_iterator, operator!=
#include <sstream>        // for basic_istringstream, basic_istream, hex
#include <string>         // for basic_string, string, char_traits
#include <utility>        // for make_pair, pair
#include <vector>         // for vector

#include "exception.hpp"  // for ParsingLineException, ParsingKeyWordAndLine...
#include "parsing.hpp"    // for realAtoi, ipv6_s, skipWSpace, ipv6_t, SEP_W...

void	parseHost(std::string &file, size_t &i, size_t &line, ip_t &ip)
{
	skipWSpace(file, i , line);
	if (std::isdigit(file[i]))
		parseIpv4(file, i, line, ip.ipv4);
	else if (file[i] == '[')
	{
		i++;
		parseIpv6(file, i, line, ip.ipv6);
	}
	else if (file.compare(i, 5, "unix:") == 0)
	{
		i += 5;
		parseIpUnix(file, i, line, ip.unix_adrr);
	}
	else
		throw (ParsingKeyWordAndLineException("Unexpected keyword", line, file.substr(i, file.find_first_of(WSPACE, i) - i)));
	skipWSpace(file, i, line);
	if (file[i] != ';')
		throw (ParsingLineException("Missing semi-colon", line));
	i++;
}

void	parseIpv4(std::string &file, size_t &i, size_t &line, std::map<in_addr_t, in_port_t> &ip)
{
	in_addr_t	ipv4;
	in_port_t	port;

	ipv4 = realAtoi(file, i, line, 255, 3) << 24;
	if (file[i] != '.')
		throw (ParsingLineException("Wrong IPv4 format", line));
	i++;
	ipv4 = ipv4 | realAtoi(file, i, line, 255, 3) << 16;
	if (file[i] != '.')
		throw (ParsingLineException("Wrong IPv4 format", line));
	i++;
	ipv4 = ipv4 | realAtoi(file, i, line, 255, 3) << 8;
	if (file[i] != '.')
		throw (ParsingLineException("Wrong IPv4 format", line));
	i++;
	ipv4 = ipv4 | realAtoi(file, i, line, 255, 3);
	parsePort(file, i, line, port);
	for (std::map<in_addr_t, in_port_t>::const_iterator it = ip.begin(); it != ip.end(); ++it)
	{
		if (ipv4 == it->first && port == it->second)
			return ;
	}
	ip.insert(std::make_pair(ipv4, port));
}

bool convertIpv6(const std::string &ip, uint8_t ipv6[16])
{
	std::vector<std::string>	parts;
	bool						begin = true;
	size_t						pos = 0;
	size_t						prev = 0;
	ssize_t						compressionPos = -1;

	// storing all the ip parts and saving the position of '::' if existing into a vecotr
	if (ip[0] == ':' && (ip[1] != ':' || ip[2] == ':'))
		return (false);
	while ((pos = ip.find(':', prev)) != std::string::npos)
	{
		if (pos > prev)
		{
			parts.push_back(ip.substr(prev, pos - prev));
			begin = false;
		}
		else if (pos == prev)
		{
			if (compressionPos != -1 && begin == false)
				return (false);
			compressionPos = parts.size();
		}
		prev = pos + 1;
	}
	parts.push_back(ip.substr(prev));

	// expanding the :: 
	if (compressionPos != -1)
	{
		int needed = 8 - (int)parts.size();
		if (needed < 1)
			return (false);
		std::vector<std::string> expanded;

		for (int i = 0; i < compressionPos; ++i)
			expanded.push_back(parts[i]);
		for (int i = 0; i < needed; ++i)
			expanded.push_back("0");
		for (size_t i = compressionPos; i < parts.size(); ++i)
			expanded.push_back(parts[i]);
		parts = expanded;
	}

	// converting the string vector of ip parts into a uint8_t[16]
	if (parts.size() != 8) 
		return (false);
	for (int i = 0; i < 8; ++i)
	{
		if (parts[i].empty())
		{
			ipv6[2*i] = 0;
			ipv6[2*i + 1] = 0;
			continue ;
		}
		if (parts[i].size() > 4)
			return (false);
		for (size_t j = 0; j < parts[i].size(); ++j)
		{
			if (!isxdigit(parts[i][j]))
				return (false);
		}
		std::istringstream iss(parts[i]);
		unsigned long val = 0;
		iss >> std::hex >> val;
		if (val > 0xFFFF)
			return (false);
		ipv6[2*i] = (uint8_t)(val >> 8);
		ipv6[2*i+1] = (uint8_t)(val & 0xFF);
	}
	return (true);
}

void	parseIpv6(std::string &file, size_t &i, size_t &line, std::map<ipv6_t, in_port_t> &ip)
{
	ipv6_t		ipv6;
	in_port_t	port;

	const size_t end = file.find(']', i);
	if (end == std::string::npos)
		throw (ParsingLineException("Expected a ']' to close ip adress", line));
	const std::string	ipString = file.substr(i, end - i);
	if (!convertIpv6(ipString, ipv6.ipv6))
		throw (ParsingKeyWordAndLineException("Wrong IPv6 format", line, ipString));
	i = end + 1;
	parsePort(file, i, line, port);
	parseScopeId(file, i, line, ipv6.scopeId);
	for (std::map<ipv6_t, in_port_t>::const_iterator it = ip.begin(); it != ip.end(); ++it)
	{
		if (ipv6 == it->first && port == it->second)
			return ;
	}
	ip.insert(std::make_pair(ipv6, port));
}

void	parseIpUnix(std::string &file, size_t &i, size_t &line, std::vector<std::string> &ip)
{
	ip.push_back(file.substr(i, file.find_first_of(SEP_WSPACE, i) - i));
	i = file.find_first_of(SEP_WSPACE, i);
	skipWSpace(file, i, line);
}

void	parsePort(std::string &file, size_t &i, size_t &line, in_port_t &port)
{
	if (file[i] != ':')
		throw (ParsingLineException("Wrong IP format", line));
	i++;
	port = realAtoi(file, i, line, 9999, 4);
}

void	parseScopeId(std::string &file, size_t &i, size_t &line, uint32_t &scopeId)
{
	if (file[i] != '%' || !std::isdigit(file[i + 1]))
		throw ParsingLineException("No scopeId in the ipv6 address", line);

	i++;
	scopeId = realAtoi(file, i, line, 9999, 4);
}
