#include <stddef.h>                 // for size_t, NULL
#include <stdint.h>                 // for uint16_t
#include <map>                      // for map, _Rb_tree_const_iterator, ope...
#include <ostream>                  // for basic_ostream, operator<<, endl
#include <string>                   // for char_traits, string, basic_string
#include <utility>                  // for pair
#include <vector>                   // for vector

#include "Route.hpp"                // for Route, operator<<
#include "ServerConfiguration.hpp"  // for ServerConfiguration, operator<<
#include "exception.hpp"            // for CustomException

ServerConfiguration::ServerConfiguration
(
	const std::vector<std::string> &serverNames,
	const std::map<unsigned short, std::string> &errorPages,
	const size_t &maxClientBodySize,
	const std::map<std::string, Route> &routes,
	const std::string &root,
	const std::vector<std::string> &index
) :
	serverNames(serverNames),
	errorPages(errorPages),
	maxClientBodySize(maxClientBodySize),
	routes(routes),
	root(root),
	index(index)
{
}

ServerConfiguration::ServerConfiguration(ServerConfiguration const &src)
{
	this->serverNames = src.serverNames;
	this->errorPages = src.errorPages;
	this->maxClientBodySize = src.maxClientBodySize;
	this->routes = src.routes;
	this->root = src.root;
	this->index = src.index;
}

ServerConfiguration::~ServerConfiguration(void)
{
	return ;
}

const std::string	&ServerConfiguration::getRoot(void) const
{
	return (this->root);
}

const std::vector<std::string>	&ServerConfiguration::getServerNames(void) const
{
	return (this->serverNames);
}

const std::string	*ServerConfiguration::getErrorPage(uint16_t errorCode) const
{
	std::map<uint16_t, std::string>::const_iterator it = this->errorPages.find(errorCode);

	if (it == this->errorPages.end())
		return (NULL);
    return (&it->second);
}

const std::map<uint16_t, std::string>	&ServerConfiguration::getErrorPages(void) const
{
	return (this->errorPages);
}

const size_t					&ServerConfiguration::getMaxClientBodySize(void) const
{
	return (this->maxClientBodySize);
}

const std::map<std::string, Route>	&ServerConfiguration::getRoutes(void) const
{
	return (this->routes);
}

const Route	*ServerConfiguration::getRouteFromPath(const std::string &path) const
{
	std::map<std::string, Route>::const_iterator temp = routes.end();

	for (std::map<std::string, Route>::const_iterator it = routes.begin(); it != routes.end(); ++it)
	{
		if (path.find(it->first) == 0 && (temp == routes.end() || temp->first.size() < it->first.size()))
			temp = it;
	}
	if (temp != routes.end())
		return (&temp->second);
	return (NULL);
}

const std::string	ServerConfiguration::getLocation(const std::string &path) const
{
	for (std::map<std::string, Route>::const_iterator it = routes.begin(); it != routes.end(); ++it)
	{
		if (path.find(it->first) == 0)
			return (it->first);
	}
	return ("");
}

const std::vector<std::string>	&ServerConfiguration::getIndex(void) const
{
	return (this->index);
}

std::ostream & operator<<(std::ostream & o, ServerConfiguration const & rhs)
{
	const std::vector<std::string>				&serverNames = rhs.getServerNames();
	const std::map<unsigned short, std::string>	&errorPages = rhs.getErrorPages();
	const std::map<std::string, Route>			&routes = rhs.getRoutes();
	const std::vector<std::string>				&index = rhs.getIndex();

	o << "root:" << rhs.getRoot() << '\n';
	o << "client_max_body_size:" << rhs.getMaxClientBodySize() << '\n';
    for (size_t i = 0; i < serverNames.size(); i++)
	{
    	o << "server_name " << i << ":" << serverNames[i] << '\n';
    }
	o << "index : ";
	for (size_t i = 0; i < index.size(); i++)
	{
		o << index[i] << " ";
	}
	o << "error pages :i\n";
	for (std::map<unsigned short, std::string>::const_iterator it = errorPages.begin(); it != errorPages.end(); ++it)
	{
		o << it->first << " : " << it->second << '\n';
    }
	o << "\nRoutes :\n";
	for (std::map<std::string, Route>::const_iterator it = routes.begin(); it != routes.end(); ++it)
	{
		o << it->first << ":\n" << it->second << '\n';
    }
	o << "\n\n";
	return (o);
}
