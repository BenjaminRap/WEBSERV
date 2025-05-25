#include <stddef.h>                 // for size_t, NULL
#include <stdint.h>                 // for uint16_t
#include <map>                      // for map, _Rb_tree_const_iterator, ope...
#include <ostream>                  // for basic_ostream, operator<<, ostream
#include <string>                   // for string, char_traits, basic_string
#include <utility>                  // for pair
#include <vector>                   // for vector

#include "Route.hpp"                // for Route, operator<<
#include "ServerConfiguration.hpp"  // for ServerConfiguration, operator<<

ServerConfiguration::ServerConfiguration
	(
		const std::vector<std::string> &serverNames,
		const std::map<unsigned short, std::string> &errorPages,
		const size_t &maxClientBodySize,
		const std::vector<EMethods> &acceptedMethods,
		const std::map<std::string, Route> &routes,
		const std::string &root,
		const std::vector<std::string> &index,
		const std::string &cgiFileExtension,
		const std::string &cgiInterpreter
	) :
	_serverNames(serverNames),
	_errorPages(errorPages),
	_maxClientBodySize(maxClientBodySize),
	_acceptedMethods(acceptedMethods),
	_routes(routes),
	_root(root),
	_index(index),
	_cgiFileExtension(cgiFileExtension),
	_cgiInterpreter(cgiInterpreter)
{
}

ServerConfiguration::ServerConfiguration(ServerConfiguration const &src)
{
	this->_serverNames = src._serverNames;
	this->_errorPages = src._errorPages;
	this->_maxClientBodySize = src._maxClientBodySize;
	this->_acceptedMethods = src._acceptedMethods;
	this->_routes = src._routes;
	this->_root = src._root;
	this->_index = src._index;
	this->_cgiFileExtension = src._cgiFileExtension;
	this->_cgiInterpreter = src._cgiInterpreter;
}

ServerConfiguration::~ServerConfiguration(void)
{
	return ;
}

const std::string	&ServerConfiguration::getRoot(void) const
{
	return (this->_root);
}

const std::vector<std::string>	&ServerConfiguration::getServerNames(void) const
{
	return (this->_serverNames);
}

const std::string	*ServerConfiguration::getErrorPage(uint16_t errorCode) const
{
	std::map<uint16_t, std::string>::const_iterator it = this->_errorPages.find(errorCode);

	if (it == this->_errorPages.end())
		return (NULL);
    return (&it->second);
}

const std::map<uint16_t, std::string>	&ServerConfiguration::getErrorPages(void) const
{
	return (this->_errorPages);
}

const size_t					&ServerConfiguration::getMaxClientBodySize(void) const
{
	return (this->_maxClientBodySize);
}

const std::vector<EMethods>		&ServerConfiguration::getAcceptedMethods(void) const
{
	return (_acceptedMethods);
}

const std::map<std::string, Route>	&ServerConfiguration::getRoutes(void) const
{
	return (this->_routes);
}

const std::pair<const std::string, Route>*	getRouteFromPath(const std::string &path, const std::map<std::string, Route>& routes)
{
	std::map<std::string, Route>::const_iterator bestMatch = routes.end();

	for (std::map<std::string, Route>::const_iterator it = routes.begin(); it != routes.end(); ++it)
	{
		if (path.rfind(it->first, 0) == 0
			&& (bestMatch == routes.end() || bestMatch->first.size() < it->first.size()))
		{
			bestMatch = it;
		}
	}
	if (bestMatch != routes.end())
	{
		return (&(*bestMatch));
	}
	return (NULL);
}

const std::pair<const std::string, Route>*	ServerConfiguration::getRouteFromPath(const std::string &path) const
{
	return (::getRouteFromPath(path, _routes));
}

const std::vector<std::string>	&ServerConfiguration::getIndex(void) const
{
	return (this->_index);
}

const std::string				&ServerConfiguration::getCgiFileExtension(void) const
{
	return (_cgiFileExtension);
}

const std::string&				ServerConfiguration::getCgiInterpreter(void) const
{
	return (_cgiInterpreter);
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
