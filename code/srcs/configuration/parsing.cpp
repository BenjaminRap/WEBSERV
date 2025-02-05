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

void	parse_file(Configuration &config, std::string &file)
{
	size_t	i = 0;
	size_t	line = 1;
	std::map<ip_t, std::vector<ServerConfiguration> >	conf;

	while (i < file.size())
	{
		skip_wspace(file, i, line);
		if (file[i] == '#')
			skip_line(file, i, line);
		else if (file.substr(i, 6) == "server")
		{
			i += 6;
			skip_wspace(file, i , line);
			if (file[i] != '{')
				throw (UnexpectedKeyWordException(line, file.substr(i, file.find_first_of(WSPACE, i) - i)));
			i++;
			parse_server(conf, file, i, line);
		}
		else
			throw (UnexpectedKeyWordException(line, file.substr(i, file.find_first_of(WSPACE, i) - i)));
		skip_wspace(file, i, line);
	}
	for (std::map<ip_t, std::vector<ServerConfiguration> >::iterator it = conf.begin(); it != conf.end(); ++it)
	{
		if (!it->first.ipv4.empty())
		{
			config.insert(std::make_pair(Host(it->first.ipv4.begin()->first, it->first.ipv4.begin()->second), it->second));
		}
		else if (!it->first.ipv6.empty())
		{
			config.insert(std::make_pair(Host(it->first.ipv6.begin()->first.getRef(), it->first.ipv6.begin()->second), it->second));
		}
		else if (!it->first.unix_adrr.empty())
		{
			config.insert(std::make_pair(Host(it->first.unix_adrr[0]), it->second));
		}
	}
}

void	parse_server(std::map<ip_t, std::vector<ServerConfiguration> > &conf, std::string &file, size_t &i, size_t &line)
{
	std::vector<std::string>				serverNames;
	std::map<unsigned short, std::string>	errorPages;
	size_t									maxClientBodySize = 0;
	std::map<std::string, Route>			routes;
	std::string								root;
	ip_t									ip;

	while (i < file.size() && file[i] != '}')
	{
		skip_wspace(file, i, line);
		if (file[i] == '#')
			skip_line(file, i, line);
		else if (file.substr(i, 6) == "listen")
		{
			i += 6;
			parse_host(file, i, line, ip);
		}
		else if (file.substr(i, 20) == "client_max_body_size")
		{
			i += 20;
			parse_maxClientBodySize(file, i, line, maxClientBodySize);
		}
		else if (file.substr(i, 11) == "server_name")
		{
			i += 11;
			parse_servername(file, i, line, serverNames);
		}
		else if (file.substr(i, 10) == "error_page")
		{
			i += 10;
			parse_errorpages(file, i, line, errorPages);
		}
		else if (file.substr(i, 8) == "location")
		{
			i += 8;
			parse_route(file, i, line, routes);
		}
		else if (file.substr(i, 4) == "root")
		{
			i += 4;
			parse_root(file, i, line, root);
		}
		else if (file[i] != '}')
			throw (UnexpectedKeyWordException(line, file.substr(i, file.find_first_of(WSPACE, i) - i)));
		skip_wspace(file, i, line);
	}
	if (file[i] != '}')
		throw (UnclosedBraceException(line));
	i++;
	if (ip.ipv4.empty() && ip.ipv6.empty() && ip.unix_adrr.empty())
		throw (MissingHostException());
	insert_host(conf, serverNames, errorPages, maxClientBodySize, routes, root, ip);
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
		throw (UnexpectedKeyWordException(line, file.substr(i, file.find_first_of(WSPACE, i) - i)));
	if (file[i] != ';')
		throw (MissingSemiColonException(line));
	i++;
}

void	parse_ipv4(std::string &file, size_t &i, size_t &line, std::map<in_addr_t, in_port_t> &ip)
{
	in_addr_t	ipv4;
	in_port_t	port;

	ipv4 = real_atoi(file, i, line, 255, 3) << 24;
	if (file[i] != '.')
		throw (WrongIpFormatException(line));
	i++;
	ipv4 = ipv4 | real_atoi(file, i, line, 255, 3) << 16;
	if (file[i] != '.')
		throw (WrongIpFormatException(line));
	i++;
	ipv4 = ipv4 | real_atoi(file, i, line, 255, 3) << 8;
	if (file[i] != '.')
		throw (WrongIpFormatException(line));
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
		if (file[i] != ':' && j != 16)
			throw (WrongIpFormatException(line));
		i++;
	}
	if (file[i] != ']')
		throw (WrongIpFormatException(line));
	parse_port(file, i, line, port);
	for (std::map<ipv6_t, in_port_t>::const_iterator it = ip.begin(); it != ip.end(); ++it)
	{
		if (ipv6.ipv6 == it->first.ipv6 && port == it->second)
			return ;
	}
	ip.insert(std::make_pair(ipv6, port));
	i++;
}

void	parse_ip_unix(std::string &file, size_t &i, size_t &line, std::vector<std::string> &ip)
{
	ip.push_back(file.substr(i, file.find_first_of(SEP_WSPACE, i) - i));
	i = file.find_first_of(SEP_WSPACE, i);
	skip_wspace(file, i, line);
	if (file[i] != ';')
		throw (MissingSemiColonException(line));
	i++;
}

void	parse_port(std::string &file, size_t &i, size_t &line, in_port_t &port)
{
	if (file[i] != ':')
		throw (WrongIpFormatException(line));
	i++;
	port = real_atoi(file, i, line, 9999, 4);
}

void	parse_maxClientBodySize(std::string &file, size_t &i, size_t &line, size_t &maxClientBodySize)
{
	if (maxClientBodySize)
		throw (MultipleDefinitionException(line, "maxClientBodySize"));
	skip_wspace(file, i , line);
	maxClientBodySize = std::atol(file.substr(i, file.find_first_not_of(DIGITS, i) - i).c_str());
	i = file.find_first_not_of(DIGITS, i);
	skip_wspace(file, i , line);
	if (file[i] != ';')
		throw (MissingSemiColonException(line));
	i++;
}

void	parse_servername(std::string &file, size_t &i, size_t &line, std::vector<std::string> &serverNames)
{
	while (i < file.size() && file[i] != ';')
	{
		skip_wspace(file, i , line);
		if (file[i] != ';')
		{
			std::string	name = file.substr(i, file.find_first_of(SEP_WSPACE, i) - i);
			if (name.find('.', 0) == std::string::npos)
				throw (WrongServerNameException(line, name));
			serverNames.push_back(name);
		}
		else
			break ;
		i = file.find_first_of(SEP_WSPACE, i);
	}
	if (file[i] != ';')
		throw (MissingSemiColonException(line));
	i++;
}

void	parse_errorpages(std::string &file, size_t &i, size_t &line, std::map<unsigned short, std::string> &errorPages)
{
	std::vector<unsigned short>	errors;
	std::string					error_page;
	
	skip_wspace(file, i, line);
	while (std::isdigit(file[i]))
	{
		errors.push_back(real_atoi(file, i, line, 999, 3));
		skip_wspace(file, i, line);
	}
	if (file[i] != '/')
		throw (MissingErrorPageException(line));
	error_page = file.substr(i, file.find_first_of(SEP_WSPACE, i) - i);
	i = file.find_first_of(SEP_WSPACE, i);
	skip_wspace(file, i, line);
	if (file[i] != ';')
		throw (MissingSemiColonException(line));
	i++;
	for (std::vector<unsigned short>::iterator it = errors.begin(); it != errors.end(); it++)
	{
		errorPages.insert(std::make_pair(*it, error_page));
	}
	if (errorPages.find(ERROR_404_INT) == errorPages.end())
		errorPages.insert(std::make_pair(ERROR_404_INT, ERROR_404_STR));
	if (errorPages.find(ERROR_405_INT) == errorPages.end())
		errorPages.insert(std::make_pair(ERROR_405_INT, ERROR_405_STR));
	if (errorPages.find(ERROR_500_INT) == errorPages.end())
		errorPages.insert(std::make_pair(ERROR_500_INT, ERROR_500_STR));
}

void	parse_route(std::string &file, size_t &i, size_t &line, std::map<std::string, Route> &routes)
{
	std::vector<EMethods>		acceptedMethods;
	std::vector<std::string>	index;
	SRedirection				redirection;
	std::string					path;
	std::string					root;
	bool						auto_index;
	std::string					cgiFileExtension;
	bool						acceptUploads;

	redirection.responseStatusCode = 0;
	skip_wspace(file, i, line);
	if (file[i] != '/')
		throw (PathNotFoundException(line));
	path = file.substr(i, file.find_first_of(SEP_WSPACE_ARG, i) - i);
	i = file.find_first_of(SEP_WSPACE_ARG, i);
	skip_wspace(file, i, line);
	if (file[i] != '{')
		throw (UnexpectedKeyWordException(line, file.substr(i, file.find_first_of(WSPACE, i) - i)));
	i++;
	while (i < file.size() && file[i] != '}')
	{
		skip_wspace(file, i, line);
		if (file.substr(i, 4) == "root")
		{
			i += 4;
			parse_root(file, i, line, root);
		}
		else if (file.substr(i, 9) == "autoindex")
		{
			i += 9;
			parse_route_autoindex(file, i, line, auto_index);
		}
		else if (file.substr(i, 5) == "index")
		{
			i += 5;
			parse_route_index(file, i, line, index);
		}
		else if (file.substr(i, 14) == "request_method")
		{
			i += 14;
			parse_route_accepted_method(file, i, line, acceptedMethods);
		}
		else if (file.substr(i, 6) == "return")
		{
			i += 6;
			parse_route_redirection(file, i, line, redirection);
		}
		else if (file.substr(i, 7) == "uploads")
		{
			i += 7;
			parse_route_uploads(file, i, line, acceptUploads);
		}
		else if (file[i] == '#')
			skip_line(file, i, line);
		else if (file[i] != '}')
			throw (UnexpectedKeyWordException(line, file.substr(i, file.find_first_of(WSPACE, i) - i)));
		skip_wspace(file, i, line);
	}
	if (file[i] != '}')
		throw (MissingSemiColonException(line));
	i++;
	if (acceptedMethods.empty())
	{
		acceptedMethods.push_back(GET);
		acceptedMethods.push_back(POST);
		acceptedMethods.push_back(DELETE);
	}
	routes.insert(std::make_pair(path, Route(acceptedMethods, redirection, index, auto_index, root, cgiFileExtension, acceptUploads)));
}

void	parse_root(std::string &file, size_t &i, size_t &line, std::string &root)
{
	skip_wspace(file, i, line);
	root = file.substr(i, file.find_first_of(SEP_WSPACE, i) - i);
	i = file.find_first_of(SEP_WSPACE, i);
	skip_wspace(file, i, line);
	if (file[i] != ';')
		throw (MissingSemiColonException(line));
	i++;
}

void	parse_route_autoindex(std::string &file, size_t &i, size_t &line, bool &auto_index)
{
	skip_wspace(file, i, line);
	if (file.substr(i, 2) == "on")
	{
		auto_index = true;
		i += 2;
	}
	else if (file.substr(i, 3) == "off")
	{
		auto_index = false;
		i += 3;
	}
	else
		throw (UnexpectedKeyWordException(line, file.substr(i, file.find_first_of(WSPACE, i) - i)));
	skip_wspace(file, i, line);
	if (file[i] != ';')
		throw (MissingSemiColonException(line));
	i++;
}

void	parse_route_index(std::string &file, size_t &i, size_t &line, std::vector<std::string> &index)
{
	while (i < file.size() && file[i] != ';')
	{
		skip_wspace(file, i, line);
		index.push_back(file.substr(i, file.find_first_of(SEP_WSPACE, i) - i));
		i = file.find_first_of(SEP_WSPACE, i);
	}
	if (file[i] != ';')
		throw (MissingSemiColonException(line));
	i++;
}

void	parse_route_accepted_method(std::string &file, size_t &i, size_t &line, std::vector<EMethods> &acceptedMethods)
{
	while (i < file.size() && file[i] != ';')
	{
		skip_wspace(file, i, line);
		if (file.substr(i, 3) == "GET")
		{
			i += 3;
			acceptedMethods.push_back(GET);
		}
		else if (file.substr(i, 4) == "POST")
		{
			i += 4;
			acceptedMethods.push_back(POST);
		}
		else if (file.substr(i, 6) == "DELETE")
		{
			i += 6;
			acceptedMethods.push_back(DELETE);
		}
		else if (file[i] != ';')
			throw (UnexpectedKeyWordException(line, file.substr(i, file.find_first_of(WSPACE, i) - i)));
	}
	if (file[i] != ';')
		throw (MissingSemiColonException(line));
	i++;
}

void	parse_route_redirection(std::string &file, size_t &i, size_t &line, SRedirection &redirection)
{
	skip_wspace(file, i, line);
	redirection.responseStatusCode = real_atoi(file, i, line, 999, 3);
	skip_wspace(file, i, line);
	redirection.url = file.substr(i, file.find_first_of(SEP_WSPACE, i) - i);
	i = file.find_first_of(SEP_WSPACE, i);
	if (file[i] != ';')
		throw (MissingSemiColonException(line));
	i++;
}

void	parse_route_uploads(std::string &file, size_t &i, size_t &line, bool &acceptUploads)
{
	skip_wspace(file, i, line);
	if (file.substr(i, 2) == "on")
	{
		acceptUploads = true;
		i += 2;
	}
	else if (file.substr(i, 3) == "off")
	{
		acceptUploads = false;
		i += 3;
	}
	else
		throw (UnexpectedKeyWordException(line, file.substr(i, file.find_first_of(WSPACE, i) - i)));
	skip_wspace(file, i, line);
	if (file[i] != ';')
		throw (MissingSemiColonException(line));
	i++;
}

void	insert_host(std::map<ip_t, std::vector<ServerConfiguration> > &conf, std::vector<std::string> \
&serverNames, std::map<unsigned short, std::string> &errorPages, size_t &maxClientBodySize, \
std::map<std::string, Route> &routes, std::string &root, ip_t &ip)
{
	for (std::map<ip_t, std::vector<ServerConfiguration> >::iterator it = conf.begin(); it != conf.end(); ++it)
	{
		if (!it->first.ipv4.empty())
		{
			for (std::map<in_addr_t, in_port_t>::iterator itt = ip.ipv4.begin(); itt != ip.ipv4.end(); ++itt)
			{
				if (it->first.ipv4.begin()->first == itt->first && it->first.ipv4.begin()->second == itt->second)
				{
					it->second.push_back(ServerConfiguration(serverNames, errorPages, maxClientBodySize, routes, root));
					ip.ipv4.erase(itt);
				}
			}
		}
		else if (!it->first.ipv6.empty())
		{
			for (std::map<ipv6_t, in_port_t>::iterator itt = ip.ipv6.begin(); itt != ip.ipv6.end(); ++itt)
			{
				if (it->first.ipv6.begin()->first == itt->first && it->first.ipv6.begin()->second == itt->second)
				{
					it->second.push_back(ServerConfiguration(serverNames, errorPages, maxClientBodySize, routes, root));
					ip.ipv6.erase(itt);
				}
			}
		}
		else if (!it->first.unix_adrr.empty())
		{
			for (std::vector<std::string>::iterator itt = ip.unix_adrr.begin(); itt != ip.unix_adrr.end(); ++itt)
			{
				if (it->first.unix_adrr[0] == *itt)
				{
					it->second.push_back(ServerConfiguration(serverNames, errorPages, maxClientBodySize, routes, root));
					ip.unix_adrr.erase(itt);
				}
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
