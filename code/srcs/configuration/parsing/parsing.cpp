#include <algorithm>                // for transform
#include <cctype>                   // for isdigit, tolower
#include <cstdlib>                  // for size_t, atol, NULL
#include <list>                     // for list
#include <map>                      // for map, _Rb_tree_iterator, _Rb_tree_...
#include <string>                   // for basic_string, string, operator==
#include <utility>                  // for pair, make_pair
#include <vector>                   // for vector

#include "Configuration.hpp"        // for Configuration
#include "EMethods.hpp"             // for EMethods
#include "Host.hpp"                 // for Host
#include "Route.hpp"                // for Route
#include "ServerConfiguration.hpp"  // for ServerConfiguration
#include "exception.hpp"            // for ParsingLineException, ParsingKeyW...
#include "parsing.hpp"              // for skipWSpace, ConfigHeaders, ip_s

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
				throw (ParsingKeyWordAndLineException("Unexpected keyword", line, file.substr(i, file.find_first_of(WSPACE, i) - i)));
			i++;
			parseServer(conf, file, i, line);
		}
		else if (i == std::string::npos)
			throw (ParsingLineException("Unexpected error", line));
		skipWSpace(file, i, line);
	}
	for (std::map<ip_t, std::vector<ServerConfiguration> >::iterator it = conf.begin(); it != conf.end(); ++it)
	{
		if (!it->first.ipv4.empty())
		{
			config.insert(std::make_pair(Host(it->first.ipv4.begin()->second, it->first.ipv4.begin()->first), it->second));
		}
		else if (!it->first.ipv6.empty())
		{
			config.insert(std::make_pair(Host(it->first.ipv6.begin()->second.getRef(), it->first.ipv6.begin()->first, it->first.ipv6.begin()->second.scopeId), it->second));
		}
		else if (!it->first.unix_adrr.empty())
		{
			config.insert(std::make_pair(Host(it->first.unix_adrr[0]), it->second));
		}
	}
}

void	replaceUrl(const std::string &location, const std::string &root, std::string &url);
const std::pair<const std::string, Route>*	getRouteFromPath(const std::string &path, const std::map<std::string, Route>& routes);

void	setErrorPagesAbsolutePath(std::map<unsigned short, std::string>& errorPages, const std::map<std::string, Route>& routes)
{
	for (std::map<unsigned short, std::string>::iterator it = errorPages.begin(); it != errorPages.end(); it++)
	{
		const std::pair<const std::string, Route>*	route = getRouteFromPath(it->second, routes);
		if (route == NULL)
			continue ;

		replaceUrl(route->first, route->second.getRoot(), it->second);
		it->second.insert(0, ".");
	}
}

void	parseServer(std::map<ip_t, std::vector<ServerConfiguration> > &conf, std::string &file, size_t &i, size_t &line)
{
	std::vector<std::string>				serverNames;
	std::map<unsigned short, std::string>	errorPages;
	size_t									maxClientBodySize = -1;
	std::vector<EMethods>					acceptedMethods;
	std::map<std::string, Route>			routes;
	std::string								root;
	ip_t									ip;
	std::vector<std::string>				index;
	std::list<ConfigHeaders> 				addHeader;
	std::string								cgiFileExtension;
	std::string								cgiInterpreter;

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
		else if (!file.compare(i, 14, "request_method"))
		{
			i += 14;
			parseRouteAcceptedMethod(file, i, line, acceptedMethods);
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
		else if (!file.compare(i, 10, "add_header"))
		{
			i += 10;
			parseAddHeader(file, i, line, addHeader);
		}
		else if (!file.compare(i, 13, "cgi_extension"))
		{
			i += 13;
			parseRouteCgiFileExtension(file, i, line, cgiFileExtension);
		}
		else if (!file.compare(i, 15, "cgi_interpreter"))
		{
			i += 15;
			parseRouteCgiInterpreter(file, i, line, cgiInterpreter);
		}
		else if (file[i] != '}')
			throw (ParsingKeyWordAndLineException("Unexpected keyword", line, file.substr(i, file.find_first_of(WSPACE, i) - i)));
		else if (i == std::string::npos)
			throw (ParsingLineException("Unexpected error", line));
		skipWSpace(file, i, line);
	}
	if (file[i] != '}')
		throw (ParsingLineException("Unclosed brace", line));
	i++;
	if (ip.ipv4.empty() && ip.ipv6.empty() && ip.unix_adrr.empty())
		throw (ParsingException("Missing host"));
	setErrorPagesAbsolutePath(errorPages, routes);
	if (acceptedMethods.empty())
	{
		acceptedMethods.push_back(GET);
		acceptedMethods.push_back(POST);
	}
	if (index.empty())
	{
		index.push_back("index.html");
		index.push_back("index.htm");
	}
	insertHost(conf, serverNames, errorPages, maxClientBodySize, acceptedMethods, routes, root, ip, index, addHeader, cgiFileExtension, cgiInterpreter);
}

void	parseMaxClientBodySize(std::string &file, size_t &i, size_t &line, size_t &maxClientBodySize)
{
	if (maxClientBodySize != (size_t)-1)
		throw (ParsingKeyWordAndLineException("Multiple definition of", line, "maxClientBodySize"));
	skipWSpace(file, i , line);
	maxClientBodySize = std::atol(file.substr(i, file.find_first_not_of(DIGITS, i) - i).c_str());
	i = file.find_first_not_of(DIGITS, i);
	skipWSpace(file, i , line);
	if (file[i] != ';')
		throw (ParsingLineException("Missing semi-colon", line));
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
				throw (ParsingKeyWordAndLineException("Wrong server name", line, name));
			serverNames.push_back(name);
		}
		else
			break ;
		i = file.find_first_of(SEP_WSPACE, i);
		if (i == std::string::npos)
			throw (ParsingLineException("Missing semi-colon", line));
	}
	if (file[i] != ';')
		throw (ParsingLineException("Missing semi-colon", line));
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
		throw (ParsingLineException("Missing error pages", line));
	error_page = file.substr(i, file.find_first_of(SEP_WSPACE, i) - i);
	i = file.find_first_of(SEP_WSPACE, i);
	skipWSpace(file, i, line);
	if (file[i] != ';')
		throw (ParsingLineException("Missing semi-colon", line));
	i++;
	for (std::vector<unsigned short>::iterator it = errors.begin(); it != errors.end(); it++)
	{
		errorPages.insert(std::make_pair(*it, error_page));
	}
}

void	fixPath(std::string &path);

void	checkRoot(std::string& root, size_t line)
{
	if (root.empty() || root[0] != '/')
		throw ParsingLineException("Invalid root", line);
	fixPath(root);
}

void	parseRoot(std::string &file, size_t &i, size_t &line, std::string &root)
{
	if (!root.empty())
		throw (ParsingLineException("Multiple definition of root", line));
	skipWSpace(file, i, line);
	root = file.substr(i, file.find_first_of(SEP_WSPACE, i) - i);
	i = file.find_first_of(SEP_WSPACE, i);
	skipWSpace(file, i, line);
	if (file[i] != ';')
		throw (ParsingLineException("Missing semi-colon", line));
	i++;
	checkRoot(root, line);
}

static char toLowerCase(char& c)
{
    return (std::tolower(c));
}

void	parseAddHeader(std::string &file, size_t &i, size_t &line, std::list<ConfigHeaders> &addHeader)
{
	std::string	key;
	std::string	value;
	bool		always = false;
	size_t		end;

	skipWSpace(file, i, line);
	end = file.find_first_of(WSPACE, i);
	if (end == std::string::npos)
		throw (ParsingLineException("Missing key", line));

	key.append(file, i, end - i);
	i = end;
	skipWSpace(file, i, line);
	if (file[i] != '"')
		throw (ParsingLineException("Missing value start quote", line));

	i++;
	end = file.find_first_of('"', i);
	if (end == std::string::npos)
		throw (ParsingLineException("Missing value end quote", line));

	value.append(file, i, end - i);
	i = end + 1;
	skipWSpace(file, i, line);
	if (file[i] != ';')
	{
		if (!file.compare(i, 6, "always"))
		{
			always = true;
			i += 6;
			skipWSpace(file, i, line);
			if (file[i] != ';')
				throw (ParsingLineException("Missing semi-colon", line));
		}
		else
			throw (ParsingLineException("Missing semi-colon", line));
	}
	i++;
	std::transform(key.begin(), key.end(), key.begin(), toLowerCase);
	addHeader.push_back(ConfigHeaders(key, value, always));
}
