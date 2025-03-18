#include <stddef.h>       // for size_t
#include <map>            // for map
#include <string>         // for basic_string, string, operator<
#include <utility>        // for make_pair
#include <vector>         // for vector

#include "EMethods.hpp"   // for EMethods
#include "Route.hpp"      // for Route, SRedirection
#include "exception.hpp"  // for CustomLineException, CustomKeyWordAndLineEx...
#include "parsing.hpp"    // for skip_wspace, WSPACE, SEP_WSPACE, SEP_WSPACE...

void	parse_route(std::string &file, size_t &i, size_t &line, std::map<std::string, Route> &routes)
{
	std::vector<EMethods>		acceptedMethods;
	std::vector<std::string>	index;
	SRedirection				redirection;
	std::string					path;
	std::string					root;
	bool						auto_index = 0;
	std::string					cgiFileExtension;
	bool						acceptUploads = 0;

	redirection.responseStatusCode = 0;
	skip_wspace(file, i, line);
	if (file[i] != '/')
		throw (CustomLineException("Path not found", line));
	path = file.substr(i, file.find_first_of(SEP_WSPACE_ARG, i) - i);
	i = file.find_first_of(SEP_WSPACE_ARG, i);
	skip_wspace(file, i, line);
	if (file[i] != '{')
		throw (CustomKeyWordAndLineException("Unexpected keyword", line, file.substr(i, file.find_first_of(WSPACE, i) - i)));
	i++;
	while (i < file.size() && file[i] != '}')
	{
		skip_wspace(file, i, line);
		if (!file.compare(i, 4, "root"))
		{
			i += 4;
			parse_root(file, i, line, root);
		}
		else if (!file.compare(i, 9, "autoindex"))
		{
			i += 9;
			parse_route_autoindex(file, i, line, auto_index);
		}
		else if (!file.compare(i, 5, "index"))
		{
			i += 5;
			parse_route_index(file, i, line, index);
		}
		else if (!file.compare(i, 14, "request_method"))
		{
			i += 14;
			parse_route_accepted_method(file, i, line, acceptedMethods);
		}
		else if (!file.compare(i, 6, "return"))
		{
			i += 6;
			parse_route_redirection(file, i, line, redirection);
		}
		else if (!file.compare(i, 7, "uploads"))
		{
			i += 7;
			parse_route_uploads(file, i, line, acceptUploads);
		}
		else if (file[i] == '#')
			skip_line(file, i, line);
		else if (file[i] != '}')
			throw (CustomKeyWordAndLineException("Unexpected keyword", line, file.substr(i, file.find_first_of(WSPACE, i) - i)));
		else if (i == std::string::npos)
			throw (CustomLineException("Unexpected error", line));
		skip_wspace(file, i, line);
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
	routes.insert(std::make_pair(path, Route(acceptedMethods, redirection, index, auto_index, root, cgiFileExtension, acceptUploads)));
}

void	parse_route_autoindex(std::string &file, size_t &i, size_t &line, bool &auto_index)
{
	skip_wspace(file, i, line);
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
	skip_wspace(file, i, line);
	if (file[i] != ';')
		throw (CustomLineException("Missing semi-colon", line));
	i++;
}

void	parse_route_index(std::string &file, size_t &i, size_t &line, std::vector<std::string> &index)
{
	while (i < file.size() && file[i] != ';')
	{
		skip_wspace(file, i, line);
		index.push_back(file.substr(i, file.find_first_of(SEP_WSPACE, i) - i));
		i = file.find_first_of(SEP_WSPACE, i);
		skip_wspace(file, i, line);
	}
	if (file[i] != ';')
		throw (CustomLineException("Missing semi-colon", line));
	i++;
}

void	parse_route_accepted_method(std::string &file, size_t &i, size_t &line, std::vector<EMethods> &acceptedMethods)
{
	while (i < file.size() && file[i] != ';')
	{
		skip_wspace(file, i, line);
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
void	parse_route_redirection(std::string &file, size_t &i, size_t &line, SRedirection &redirection)
{
	skip_wspace(file, i, line);
	redirection.responseStatusCode = real_atoi(file, i, line, 999, 3);
	skip_wspace(file, i, line);
	redirection.url = file.substr(i, file.find_first_of(SEP_WSPACE, i) - i);
	i = file.find_first_of(SEP_WSPACE, i);
	if (file[i] != ';')
		throw (CustomLineException("Missing semi-colon", line));
	i++;
}

void	parse_route_uploads(std::string &file, size_t &i, size_t &line, bool &acceptUploads)
{
	skip_wspace(file, i, line);
	if (!file.compare(i, 2, "on"))
	{
		acceptUploads = true;
		i += 2;
	}
	else if (!file.compare(i, 3, "off"))
	{
		acceptUploads = false;
		i += 3;
	}
	else
		throw (CustomKeyWordAndLineException("Unexpected keyword", line, file.substr(i, file.find_first_of(WSPACE, i) - i)));
	skip_wspace(file, i, line);
	if (file[i] != ';')
		throw (CustomLineException("Missing semi-colon", line));
	i++;
}
