#include "ServerConfiguration.hpp"
#include "exception.hpp"

ServerConfiguration::ServerConfiguration(	const std::vector<std::string> &serverNames, \
											const std::map<unsigned short, std::string> &errorPages, \
											const size_t &maxClientBodySize, \
											const std::map<std::string, Route> &routes, \
											const std::string &root, \
											const std::vector<std::string> &index) : \
											serverNames(serverNames), \
											errorPages(errorPages), \
											maxClientBodySize(maxClientBodySize), \
											routes(routes), \
											root(root), \
											index(index)
{
	return ;
}

ServerConfiguration::ServerConfiguration(ServerConfiguration const &src)
{
	*this = src;
}

ServerConfiguration    &ServerConfiguration::operator=(ServerConfiguration const &src)
{
	if (this != &src)
	{
		this->serverNames = src.serverNames;
		this->errorPages = src.errorPages;
		this->maxClientBodySize = src.maxClientBodySize;
		this->routes = src.routes;
		this->root = src.root;
		this->index = src.index;
	}
	return (*this);
}

ServerConfiguration::~ServerConfiguration(void)
{
	return ;
}

const std::string				&ServerConfiguration::getRoot(void) const
{
	return (this->root);
}

const std::vector<std::string>	&ServerConfiguration::getServerNames(void) const
{
	return (this->serverNames);
}

const std::string				&ServerConfiguration::getErrorPage(unsigned short error) const
{
	std::map<unsigned short, std::string>::const_iterator it = this->errorPages.find(error);

	if (it == this->errorPages.end())
		throw (CustomException("Non existing error_page"));
    return (it->second);
}

const std::map<unsigned short, std::string>	&ServerConfiguration::getErrorPages(void) const
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

const Route	*ServerConfiguration::getOneRoutes(const std::string &path) const
{
	for (std::map<std::string, Route>::const_iterator it = routes.begin(); it != routes.end(); ++it)
	{
		if (path.find(it->first) == 0)
			return (&it->second);
	}
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

const std::vector<std::string>				&ServerConfiguration::getIndex(void) const
{
	return (this->index);
}

std::ostream & operator<<(std::ostream & o, ServerConfiguration const & rhs)
{
	const std::vector<std::string>				&serverNames = rhs.getServerNames();
	const std::map<unsigned short, std::string>	&errorPages = rhs.getErrorPages();
	const std::map<std::string, Route>			&routes = rhs.getRoutes();
	const std::vector<std::string>				&index = rhs.getIndex();

	o << "root:" << rhs.getRoot() << std::endl;
	o << "client_max_body_size:" << rhs.getMaxClientBodySize() << std::endl;
    for (size_t i = 0; i < serverNames.size(); i++)
	{
    	o << "server_name " << i << ":" << serverNames[i] << std::endl;
    }
	o << "index : ";
	for (size_t i = 0; i < index.size(); i++)
	{
		o << index[i] << " ";
	}
	o << "error pages :" << std::endl;
	for (std::map<unsigned short, std::string>::const_iterator it = errorPages.begin(); it != errorPages.end(); ++it)
	{
		o << it->first << " : " << it->second << std::endl;
    }
	o << std::endl << "Routes :" << std::endl;
	for (std::map<std::string, Route>::const_iterator it = routes.begin(); it != routes.end(); ++it)
	{
		o << it->first << ":" << std::endl << it->second << std::endl;
    }
	o << std::endl << std::endl;
	return (o);
}
