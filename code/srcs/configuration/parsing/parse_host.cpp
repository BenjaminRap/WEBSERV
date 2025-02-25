#include "parsing.hpp"

void	parse_host(std::string &file, size_t &i, size_t &line, ip_t &ip)
{
	skip_wspace(file, i , line);
	if (std::isdigit(file[i]))
		parse_ipv4(file, i, line, ip.ipv4);
	else if (file[i] == '[')
	{
		i++;
		parse_ipv6(file, i, line, ip.ipv6);
	}
	else if (file.substr(i, 5) == "unix:")
	{
		i += 5;
		parse_ip_unix(file, i, line, ip.unix_adrr);
	}
	else
		throw (CustomKeyWordAndLineException("Unexpected keyword", line, file.substr(i, file.find_first_of(WSPACE, i) - i)));
	skip_wspace(file, i, line);
	if (file[i] != ';')
		throw (CustomLineException("Missing semi-colon", line));
	i++;
}

void	parse_ipv4(std::string &file, size_t &i, size_t &line, std::map<in_addr_t, in_port_t> &ip)
{
	in_addr_t	ipv4;
	in_port_t	port;

	ipv4 = real_atoi(file, i, line, 255, 3) << 24;
	if (file[i] != '.')
		throw (CustomLineException("Wrong IP format", line));
	i++;
	ipv4 = ipv4 | real_atoi(file, i, line, 255, 3) << 16;
	if (file[i] != '.')
		throw (CustomLineException("Wrong IP format", line));
	i++;
	ipv4 = ipv4 | real_atoi(file, i, line, 255, 3) << 8;
	if (file[i] != '.')
		throw (CustomLineException("Wrong IP format", line));
	i++;
	ipv4 = ipv4 | real_atoi(file, i, line, 255, 3);
	parse_port(file, i, line, port);
	for (std::map<in_addr_t, in_port_t>::const_iterator it = ip.begin(); it != ip.end(); ++it)
	{
		if (ipv4 == it->first && port == it->second)
			return ;
	}
	ip.insert(std::make_pair(ipv4, port));
}

void	parse_ipv6(std::string &file, size_t &i, size_t &line, std::map<ipv6_t, in_port_t> &ip)
{
	ipv6_t		ipv6;
	in_port_t	port;

	for (int j = 0; j < 16; j++)
	{
		ipv6.ipv6[j] = ft_hextoint(file, i, line);
		j++;
		ipv6.ipv6[j] = ft_hextoint(file, i, line);
		if (file[i] != ':' && j != 15)
			throw (CustomLineException("Wrong IP format", line));
		if (j != 15)
			i++;
	}
	if (file[i] != ']')
		throw (CustomLineException("Wrong IP format", line));
	i++;
	parse_port(file, i, line, port);
	for (std::map<ipv6_t, in_port_t>::const_iterator it = ip.begin(); it != ip.end(); ++it)
	{
		if (ipv6 == it->first && port == it->second)
			return ;
	}
	ip.insert(std::make_pair(ipv6, port));
}

void	parse_ip_unix(std::string &file, size_t &i, size_t &line, std::vector<std::string> &ip)
{
	ip.push_back(file.substr(i, file.find_first_of(SEP_WSPACE, i) - i));
	i = file.find_first_of(SEP_WSPACE, i);
	skip_wspace(file, i, line);
}

void	parse_port(std::string &file, size_t &i, size_t &line, in_port_t &port)
{
	if (file[i] != ':')
		throw (CustomLineException("Wrong IP format", line));
	i++;
	port = real_atoi(file, i, line, 9999, 4);
}
