#include <stddef.h>       // for size_t
#include <map>            // for map
#include <string>         // for basic_string, string, operator<
#include <utility>        // for make_pair
#include <vector>         // for vector

#include "EMethods.hpp"   // for EMethods
#include "Route.hpp"      // for Route, SRedirection
#include "exception.hpp"  // for CustomLineException, CustomKeyWordAndLineEx...
#include "parsing.hpp"    // for skip_wspace, WSPACE, SEP_WSPACE, SEP_WSPACE...

void	parseRoute(std::string &file, size_t &i, size_t &line, std::map<std::string, Route> &routes)
{
	std::vector<EMethods>		acceptedMethods;
	std::vector<std::string>	index;
	SRedirection				redirection;
	std::string					path;
	std::string					root;
	bool						auto_index = 0;
	std::string					cgiFileExtension;

	redirection.responseStatusCode = 0;
	skipWSpace(file, i, line);
	if (file[i] != '/')
		throw (CustomLineException("Path not found", line));
	path = file.substr(i, file.find_first_of(SEP_WSPACE_ARG, i) - i);
	i = file.find_first_of(SEP_WSPACE_ARG, i);
	skipWSpace(file, i, line);
	if (file[i] != '{')
		throw (CustomKeyWordAndLineException("Unexpected keyword", line, file.substr(i, file.find_first_of(WSPACE, i) - i)));
	i++;
	while (i < file.size() && file[i] != '}')
	{
		skipWSpace(file, i, line);
		if (!file.compare(i, 4, "root"))
		{
			i += 4;
			parseRoot(file, i, line, root);
		}
		else if (!file.compare(i, 9, "autoindex"))
		{
			i += 9;
			parseRouteAutoIndex(file, i, line, auto_index);
		}
		else if (!file.compare(i, 5, "index"))
		{
			i += 5;
			parseRouteIndex(file, i, line, index);
		}
		else if (!file.compare(i, 14, "request_method"))
		{
			i += 14;
			parseRouteAcceptedMethod(file, i, line, acceptedMethods);
		}
		else if (!file.compare(i, 6, "return"))
		{
			i += 6;
			parseRouteRedirection(file, i, line, redirection);
		}
		else if (!file.compare(i, 13, "cgi_extension"))
		{
			i += 13;
			parseRouteCgiFileExtension(file, i, line, cgiFileExtension);
		}
		else if (file[i] == '#')
			skipLine(file, i, line);
		else if (file[i] != '}')
			throw (CustomKeyWordAndLineException("Unexpected keyword", line, file.substr(i, file.find_first_of(WSPACE, i) - i)));
		else if (i == std::string::npos)
			throw (CustomLineException("Unexpected error", line));
		skipWSpace(file, i, line);
	}
	if (file[i] != '}')
		throw (CustomLineException("Unclosed brace", line));
	i++;
	if (acceptedMethods.empty())
	{
		acceptedMethods.push_back(GET);
		acceptedMethods.push_back(POST);
		acceptedMethods.push_back(DELETE);
	}
	routes.insert(std::make_pair(path, Route(acceptedMethods, redirection, index, auto_index, root, cgiFileExtension)));
}

void	parseRouteAutoIndex(std::string &file, size_t &i, size_t &line, bool &auto_index)
{
	skipWSpace(file, i, line);
	if (!file.compare(i, 2, "on"))
	{
		auto_index = true;
		i += 2;
	}
	else if (!file.compare(i, 3, "off"))
	{
		auto_index = false;
		i += 3;
	}
	else
		throw (CustomKeyWordAndLineException("Unexpected keyword", line, file.substr(i, file.find_first_of(WSPACE, i) - i)));
	skipWSpace(file, i, line);
	if (file[i] != ';')
		throw (CustomLineException("Missing semi-colon", line));
	i++;
}

void	parseRouteIndex(std::string &file, size_t &i, size_t &line, std::vector<std::string> &index)
{
	while (i < file.size() && file[i] != ';')
	{
		skipWSpace(file, i, line);
		index.push_back(file.substr(i, file.find_first_of(SEP_WSPACE, i) - i));
		i = file.find_first_of(SEP_WSPACE, i);
		skipWSpace(file, i, line);
	}
	if (file[i] != ';')
		throw (CustomLineException("Missing semi-colon", line));
	i++;
}

void	parseRouteAcceptedMethod(std::string &file, size_t &i, size_t &line, std::vector<EMethods> &acceptedMethods)
{
	while (i < file.size() && file[i] != ';')
	{
		skipWSpace(file, i, line);
		if (!file.compare(i, 3, "GET"))
		{
			i += 3;
			acceptedMethods.push_back(GET);
		}
		else if (!file.compare(i, 4, "POST"))
		{
			i += 4;
			acceptedMethods.push_back(POST);
		}
		else if (!file.compare(i, 6, "DELETE"))
		{
			i += 6;
			acceptedMethods.push_back(DELETE);
		}
		else if (file[i] != ';')
			throw (CustomKeyWordAndLineException("Unexpected keyword", line, file.substr(i, file.find_first_of(WSPACE, i) - i)));
	}
	if (file[i] != ';')
		throw (CustomLineException("Missing semi-colon", line));
	i++;
}
void	parseRouteRedirection(std::string &file, size_t &i, size_t &line, SRedirection &redirection)
{
	skipWSpace(file, i, line);
	redirection.responseStatusCode = realAtoi(file, i, line, 999, 3);
	skipWSpace(file, i, line);
	redirection.url = file.substr(i, file.find_first_of(SEP_WSPACE, i) - i);
	i = file.find_first_of(SEP_WSPACE, i);
	if (file[i] != ';')
		throw (CustomLineException("Missing semi-colon", line));
	i++;
}

void	parseRouteCgiFileExtension(std::string &file, size_t &i, size_t &line, std::string &cgiFileExtention)
{
	skipWSpace(file, i, line);
	cgiFileExtention = file.substr(i, file.find_first_of(WSPACE, i) - i);
	skipWSpace(file, i, line);
	if (file[i] != ';')
		throw (CustomLineException("Missing semi-colon", line));
	i++;
}
