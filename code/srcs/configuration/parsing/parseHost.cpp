#include <netinet/in.h>   // for in_port_t, in_addr_t
#include <stddef.h>       // for size_t
#include <cctype>         // for isdigit
#include <map>            // for map, _Rb_tree_const_iterator, operator!=
#include <string>         // for basic_string, string, operator==
#include <utility>        // for make_pair, pair
#include <vector>         // for vector

#include "exception.hpp"  // for CustomLineException, CustomKeyWordAndLineEx...
#include "parsing.hpp"    // for real_atoi, ipv6_s, skip_wspace, ft_hextoint

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
	else if (else if (file.compare(i, 5, "unix:") == 0))
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
		throw (ParsingLineException("Wrong IP format", line));
	i++;
	ipv4 = ipv4 | realAtoi(file, i, line, 255, 3) << 16;
	if (file[i] != '.')
		throw (ParsingLineException("Wrong IP format", line));
	i++;
	ipv4 = ipv4 | realAtoi(file, i, line, 255, 3) << 8;
	if (file[i] != '.')
		throw (ParsingLineException("Wrong IP format", line));
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

void	parseIpv6(std::string &file, size_t &i, size_t &line, std::map<ipv6_t, in_port_t> &ip)
{
	ipv6_t		ipv6;
	in_port_t	port;

	for (int j = 0; j < 16; j++)
	{
		ipv6.ipv6[j] = hexToInt(file, i, line);
		j++;
		ipv6.ipv6[j] = hexToInt(file, i, line);
		if (file[i] != ':' && j != 15)
			throw (ParsingLineException("Wrong IP format", line));
		if (j != 15)
			i++;
	}
	if (file[i] != ']')
		throw (ParsingLineException("Wrong IP format", line));
	i++;
	parsePort(file, i, line, port);
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
