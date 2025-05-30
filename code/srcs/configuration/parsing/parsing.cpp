#include <cctype>                   // for isdigit
#include <cstdlib>                  // for size_t, atol
#include <map>                      // for map, _Rb_tree_iterator, operator==
#include <string>                   // for basic_string, string, operator==
#include <utility>                  // for make_pair, pair
#include <vector>                   // for vector

#include "Configuration.hpp"        // for Configuration
#include "Host.hpp"                 // for Host
#include "Route.hpp"                // for Route
#include "ServerConfiguration.hpp"  // for ServerConfiguration
#include "exception.hpp"            // for CustomLineException, CustomKeyWor...
#include "parsing.hpp"              // for skip_wspace, ip_s, SEP_WSPACE, ip_t

void	parseFile(Configuration &config, std::string &file)
{
	size_t	i = 0;
	size_t	line = 1;
	std::map<ip_t, std::vector<ServerConfiguration> >	conf;

	while (i < file.size())
	{
		skipWSpace(file, i, line);
		if (file[i] == '#')
			skipLine(file, i, line);
		else if (file.substr(i, 6) == "server")
		{
			i += 6;
			skipWSpace(file, i , line);
			if (file[i] != '{')
				throw (CustomKeyWordAndLineException("Unexpected keyword", line, file.substr(i, file.find_first_of(WSPACE, i) - i)));
			i++;
			parseServer(conf, file, i, line);
		}
		else if (i == std::string::npos)
			throw (CustomLineException("Unexpected error", line));
		skipWSpace(file, i, line);
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

void	parseServer(std::map<ip_t, std::vector<ServerConfiguration> > &conf, std::string &file, size_t &i, size_t &line)
{
	std::vector<std::string>				serverNames;
	std::map<unsigned short, std::string>	errorPages;
	size_t									maxClientBodySize = -1;
	std::map<std::string, Route>			routes;
	std::string								root;
	ip_t									ip;
	std::vector<std::string>				index;

	while (i < file.size() && file[i] != '}')
	{
		skipWSpace(file, i, line);
		if (file[i] == '#')
			skipLine(file, i, line);
		else if (!file.compare(i, 6, "listen"))
		{
			i += 6;
			parseHost(file, i, line, ip);
		}
		else if (!file.compare(i, 20, "client_max_body_size"))
		{
			i += 20;
			parseMaxClientBodySize(file, i, line, maxClientBodySize);
		}
		else if (!file.compare(i, 11, "server_name"))
		{
			i += 11;
			parseServerName(file, i, line, serverNames);
		}
		else if (!file.compare(i, 10, "error_page"))
		{
			i += 10;
			parseErrorPages(file, i, line, errorPages);
		}
		else if (!file.compare(i, 8, "location"))
		{
			i += 8;
			parseRoute(file, i, line, routes);
		}
		else if (!file.compare(i, 4, "root"))
		{
			i += 4;
			parseRoot(file, i, line, root);
		}
		else if (!file.compare(i, 5, "index"))
		{
			i += 5;
			parseRouteIndex(file, i, line, index);
		}
		else if (file[i] != '}')
			throw (CustomKeyWordAndLineException("Unexpected keyword", line, file.substr(i, file.find_first_of(WSPACE, i) - i)));
		else if (i == std::string::npos)
			throw (CustomLineException("Unexpected error", line));
		skipWSpace(file, i, line);
	}
	if (file[i] != '}')
		throw (CustomLineException("Unclosed brace", line));
	i++;
	if (ip.ipv4.empty() && ip.ipv6.empty() && ip.unix_adrr.empty())
		throw (CustomException("Missing host"));
	insertHost(conf, serverNames, errorPages, maxClientBodySize, routes, root, ip, index);
}

void	parseMaxClientBodySize(std::string &file, size_t &i, size_t &line, size_t &maxClientBodySize)
{
	if (maxClientBodySize != (size_t)-1)
		throw (CustomKeyWordAndLineException("Multiple definition of", line, "maxClientBodySize"));
	skipWSpace(file, i , line);
	maxClientBodySize = std::atol(file.substr(i, file.find_first_not_of(DIGITS, i) - i).c_str());
	i = file.find_first_not_of(DIGITS, i);
	skipWSpace(file, i , line);
	if (file[i] != ';')
		throw (CustomLineException("Missing semi-colon", line));
	i++;
}

void	parseServerName(std::string &file, size_t &i, size_t &line, std::vector<std::string> &serverNames)
{
	while (i < file.size() && file[i] != ';')
	{
		skipWSpace(file, i , line);
		if (file[i] != ';')
		{
			std::string	name = file.substr(i, file.find_first_of(SEP_WSPACE, i) - i);
			if (name.find('.', 0) == std::string::npos)
				throw (CustomKeyWordAndLineException("Wrong server name", line, name));
			serverNames.push_back(name);
		}
		else
			break ;
		i = file.find_first_of(SEP_WSPACE, i);
		if (i == std::string::npos)
			throw (CustomLineException("Missing semi-colon", line));
	}
	if (file[i] != ';')
		throw (CustomLineException("Missing semi-colon", line));
	i++;
}

void	parseErrorPages(std::string &file, size_t &i, size_t &line, std::map<unsigned short, std::string> &errorPages)
{
	std::vector<unsigned short>	errors;
	std::string					error_page;
	
	skipWSpace(file, i, line);
	while (std::isdigit(file[i]))
	{
		errors.push_back(realAtoi(file, i, line, 999, 3));
		skipWSpace(file, i, line);
	}
	if (file[i] != '/')
		throw (CustomLineException("Missing error pages", line));
	error_page = file.substr(i, file.find_first_of(SEP_WSPACE, i) - i);
	i = file.find_first_of(SEP_WSPACE, i);
	skipWSpace(file, i, line);
	if (file[i] != ';')
		throw (CustomLineException("Missing semi-colon", line));
	i++;
	for (std::vector<unsigned short>::iterator it = errors.begin(); it != errors.end(); it++)
	{
		errorPages.insert(std::make_pair(*it, error_page));
	}
}

void	parseRoot(std::string &file, size_t &i, size_t &line, std::string &root)
{
	if (!root.empty())
		throw (CustomLineException("Multiple definition of root", line));
	skipWSpace(file, i, line);
	root = file.substr(i, file.find_first_of(SEP_WSPACE, i) - i);
	i = file.find_first_of(SEP_WSPACE, i);
	skipWSpace(file, i, line);
	if (file[i] != ';')
		throw (CustomLineException("Missing semi-colon", line));
	i++;
}
