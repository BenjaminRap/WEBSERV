#include "parsing.hpp"

void	ft_readfile(const char *path, std::string &buff)
{
	std::ifstream	file;

	file.open(path, file.in);
	if (file.fail())
		throw (OpenFileException());
	if (!std::getline(file, buff, '\0'))
		throw (ReadFileException());
}

void	skip_line(std::string &file, size_t &i, size_t &line)
{
	i = file.find('\n', i);
	if (i != file.size())
		line++;
	i++;
}

void	skip_wspace(std::string &file, size_t &i, size_t &line)
{
	while (std::isspace(static_cast<unsigned char>(file[i])))
	{
		if (file[i] == '\n')
			line++;
		i++;
	}
}

short	real_atoi(std::string &file, size_t &i, size_t &line, short max, short len)
{
	short	nb = 0;

	while (len-- > 0 && std::isdigit(file[i]))
	{
		nb = nb * 10 + (file[i] - '0');
		i++;
		len++;
	}
	if (nb > max || nb < 0)
		throw (ParsedNumberOutOfRangeException(line));
	return (nb);
}

uint8_t	ft_hextoint(std::string &file, size_t &i, size_t &line)
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
		throw (WrongIpFormatException(line));
	return (nb);
}

void	insert_host(std::map<ip_t, std::vector<ServerConfiguration> > &conf, std::vector<std::string> \
&serverNames, std::map<unsigned short, std::string> &errorPages, size_t &maxClientBodySize, \
std::map<std::string, Route> &routes, std::string &root, ip_t &ip)
{
	for (std::map<ip_t, std::vector<ServerConfiguration> >::iterator it = conf.begin(); it != conf.end(); ++it)
	{
		if (!it->first.ipv4.empty())
		{
			for (std::map<in_addr_t, in_port_t>::iterator itt = ip.ipv4.begin(); itt != ip.ipv4.end();)
			{
				if (it->first.ipv4.begin()->first == itt->first && it->first.ipv4.begin()->second == itt->second)
				{
					it->second.push_back(ServerConfiguration(serverNames, errorPages, maxClientBodySize, routes, root));
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
					it->second.push_back(ServerConfiguration(serverNames, errorPages, maxClientBodySize, routes, root));
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
					it->second.push_back(ServerConfiguration(serverNames, errorPages, maxClientBodySize, routes, root));
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

		serv.push_back(ServerConfiguration(serverNames, errorPages, maxClientBodySize, routes, root));
		temp.ipv4.insert(*it);
		conf.insert(std::make_pair(temp, serv));
	}
	for (std::map<ipv6_t, in_port_t>::iterator it = ip.ipv6.begin(); it != ip.ipv6.end(); ++it)
	{
		ip_t	temp;
		std::vector<ServerConfiguration> serv;

		serv.push_back(ServerConfiguration(serverNames, errorPages, maxClientBodySize, routes, root));
		temp.ipv6.insert(*it);
		conf.insert(std::make_pair(temp, serv));
	}
	for (std::vector<std::string>::iterator it = ip.unix_adrr.begin(); it != ip.unix_adrr.end(); ++it)
	{
		ip_t	temp;
		std::vector<ServerConfiguration> serv;

		serv.push_back(ServerConfiguration(serverNames, errorPages, maxClientBodySize, routes, root));
		temp.unix_adrr.push_back(*it);
		conf.insert(std::make_pair(temp, serv));
	}
}
