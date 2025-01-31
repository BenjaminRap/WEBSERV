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
			parse_server(config, file, i, line);
		}
		else
			throw (UnexpectedKeyWordException(line, file.substr(i, file.find_first_of(WSPACE, i) - i)));
		skip_wspace(file, i, line);
	}
}

void	parse_server(Configuration &config, std::string &file, size_t &i, size_t &line)
{
	std::vector<std::string>				serverNames;
	std::map<unsigned short, std::string>	errorPages;
	size_t									maxClientBodySize = 0;
	std::map<std::string, Route>			routes;
	std::string								root;
	ip										ip;

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
		throw (MissingSemiColonException(line));
	i++;
	config.push_back(ServerConfiguration(serverNames, errorPages, maxClientBodySize, routes, root));
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

void	parse_host(std::string &file, size_t &i, size_t &line, ip &ip)
{
	skip_wspace(file, i , line);
	if (std::isdigit(file[i]))
		parse_ipv4(file, i, line, ip.ipv4);
	if (file[i] != ';')
		throw (MissingSemiColonException(line));
}

void	parse_ip(std::string &file, size_t &i, size_t &line, uint32_t &host)
{

}

void	parse_ip_unix(std::string &file, size_t &i, size_t &line, std::vector<std::string> &ip)
{

}

void	parse_ipv4(std::string &file, size_t &i, size_t &line, std::map<in_addr_t, in_port_t> &ip)
{
	in_addr_t	ipv4;

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
}

void	parse_ipv6(std::string &file, size_t &i, size_t &line, std::map<uint8_t [16], in_port_t> &ip)
{

}


void	parse_port(std::string &file, size_t &i, size_t &line, uint16_t &port)
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