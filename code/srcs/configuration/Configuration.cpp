#include "Configuration.hpp"

Configuration::Configuration(void)
{
	return ;
}

Configuration::Configuration(const char *path)
{
	std::string buff;

	ft_readfile(path, buff);
	parse_file(buff);
	return ;
}

Configuration::~Configuration(void)
{
	return ;
}

void	Configuration::parse_file(std::string &file)
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
			parse_server(file, i, line);
		}
		else
			throw (UnexpectedKeyWordException(line, file.substr(i, file.find_first_of(WSPACE, i) - i)));
	}
}

void	Configuration::parse_server(std::string &file, size_t &i, size_t &line)
{
	uint32_t								host = 0;
	uint16_t								port = 0;
	std::vector<std::string>				serverNames;
	std::map<unsigned short, std::string>	errorPages;
	size_t									maxClientBodySize = 0;
	std::map<std::string, Route>			routes;

	i++;
	while (i < file.size() && file[i] != '}')
	{
		skip_wspace(file, i, line);
		if (file[i] == '#')
			skip_line(file, i, line);
		else if (file.substr(i, 6) == "listen")
		{
			i += 6;
			if (host || port)
				throw (MultipleDefinitionException(line, "host/port"));
			skip_wspace(file, i , line);
			parse_ip(file, i, line, host);
			parse_port(file, i, line, port);
			skip_wspace(file, i , line);
			if (file[i] != ';')
				throw (MissingSemiColonException(line));
			i++;
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
			//----------print server config--------------//		
			std::cout << "host:" << (host >> 24) << ((host << 8) >> 24) << ((host << 16) >> 24) << ((host << 24) >> 24) << std::endl;
			std::cout << "port:" << port << std::endl;
			std::cout << "client_max_body_size:" << maxClientBodySize << std::endl;
    		for (std::vector<std::string>::iterator it = serverNames.begin(); it != serverNames.end(); ++it)
			{
				static int i = 1;
        		std::cout << "server_name " << i << ":" << *it << std::endl;
				i++;
    		}
			std::cout << "error pages :" << std::endl;
			for (std::map<unsigned short, std::string>::iterator it = errorPages.begin(); it != errorPages.end(); it++)
			{
				std::cout << it->first << " : " << it->second << std::endl;
    		}
			std::cout << std::endl << "Routes :" << std::endl;
			for (std::map<std::string, Route>::iterator it = routes.begin(); it != routes.end(); it++)
			{
				std::cout << it->first << ":" << std::endl << it->second << std::endl;
    		}
			//----------print server config--------------//
		}
		i++;
	}
}

void	Configuration::skip_line(std::string &file, size_t &i, size_t &line)
{
	i = file.find('\n', i);
	if (i != file.size())
		line++;
	i++;
}

void	Configuration::skip_wspace(std::string &file, size_t &i, size_t &line)
{
	while (std::isspace(static_cast<unsigned char>(file[i])))
	{
		if (file[i] == '\n')
			line++;
		i++;
	}
}

short	Configuration::real_atoi(std::string &file, size_t &i, size_t &line, short max, short len)
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

void	Configuration::parse_ip(std::string &file, size_t &i, size_t &line, uint32_t &host)
{
	host = real_atoi(file, i, line, 255, 3) << 24;
	if (file[i] != '.')
		throw (WrongIpFormatException(line));
	i++;
	host = host | real_atoi(file, i, line, 255, 3) << 16;
	if (file[i] != '.')
		throw (WrongIpFormatException(line));
	i++;
	host = host | real_atoi(file, i, line, 255, 3) << 8;
	if (file[i] != '.')
		throw (WrongIpFormatException(line));
	i++;
	host = host | real_atoi(file, i, line, 255, 3);
}

void	Configuration::parse_port(std::string &file, size_t &i, size_t &line, uint16_t &port)
{
	if (file[i] != ':')
		throw (WrongIpFormatException(line));
	i++;
	port = real_atoi(file, i, line, 9999, 4);
}

void	Configuration::parse_maxClientBodySize(std::string &file, size_t &i, size_t &line, size_t &maxClientBodySize)
{
	if (maxClientBodySize)
		throw (MultipleDefinitionException(line, "maxClientBodySize"));
	skip_wspace(file, i , line);
	maxClientBodySize = std::atol(file.substr(i, file.find_first_not_of(DIGITS, i) - i).c_str());
	i = file.find_first_not_of(DIGITS, i);
	skip_wspace(file, i , line);
	if (file[i] != ';')
		throw (MissingSemiColonException(line));
}

void	Configuration::parse_servername(std::string &file, size_t &i, size_t &line, std::vector<std::string> &serverNames)
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
}

void	Configuration::parse_errorpages(std::string &file, size_t &i, size_t &line, std::map<unsigned short, std::string> &errorPages)
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
	for (std::vector<unsigned short>::iterator it = errors.begin(); it != errors.end(); it++)
	{
		errorPages.insert(std::make_pair(*it, error_page));
	}
}

void	Configuration::parse_route(std::string &file, size_t &i, size_t &line, std::map<std::string, Route> &routes)
{
	std::vector<EMethods>		acceptedMethods;
	std::vector<std::string>	index;
	SRedirection				redirection;
	std::string					path;
	std::string					root;
	bool						directoryListing = false;
	bool						auto_index;
	std::string					directoryFile;
	std::string					cgiFileExtension;
	SUploads					uploads;

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
			skip_wspace(file, i, line);
			root = file.substr(i, file.find_first_of(SEP_WSPACE, i) - i);
			i = file.find_first_of(SEP_WSPACE, i);
			skip_wspace(file, i, line);
			if (file[i] != ';')
				throw (MissingSemiColonException(line));
			i++;
		}
		else if (file.substr(i, 9) == "autoindex")
		{
			i += 9;
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
		}
		else if (file.substr(i, 5) == "index")
		{
			i += 5;
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
		else if (file.substr(i, 14) == "request_method")
		{
			i += 14;
		}
		else if (file.substr(i, 6) == "return")
		{
			i += 6;
		}
		else if (file[i] == '#')
			skip_line(file, i, line);
		else
			throw (UnexpectedKeyWordException(line, file.substr(i, file.find_first_of(WSPACE, i) - i)));
		skip_wspace(file, i, line);
	}
	if (file[i] != '}')
		throw (MissingSemiColonException(line));
	routes.insert(std::make_pair(path, Route(acceptedMethods, redirection, index, auto_index, root, directoryListing, directoryFile, cgiFileExtension, uploads)));
}
