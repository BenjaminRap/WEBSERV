#include "ServerConfiguration.hpp"

ServerConfiguration::ServerConfiguration(	std::vector<std::string> serverNames, \
											std::map<unsigned short, std::string> errorPages, \
											size_t maxClientBodySize, \
											std::map<std::string, Route> routes, \
											std::string root) : \
											serverNames(serverNames), \
											errorPages(errorPages), \
											maxClientBodySize(maxClientBodySize), \
											routes(routes), \
											root(root) \
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
    return (this->errorPages.find(error)->second);
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

const Route	*ServerConfiguration::getOneRoutes(std::string path) const
{
	std::cout << path << std::endl;
	for (std::map<std::string, Route>::const_iterator it = routes.begin(); it != routes.end(); ++it)
	{
		if (it->first.find(path) == 0)
			return (&it->second);
	}
	return (NULL);
}

std::ostream & operator<<(std::ostream & o, ServerConfiguration const & rhs)
{
	const std::vector<std::string>				&serverNames = rhs.getServerNames();
	const std::map<unsigned short, std::string>	&errorPages = rhs.getErrorPages();
	const std::map<std::string, Route>			&routes = rhs.getRoutes();

	o << "SERVER :" << std::endl;		
	o << "root:" << rhs.getRoot() << std::endl;
	o << "client_max_body_size:" << rhs.getMaxClientBodySize() << std::endl;
    for (size_t i = 0; i < serverNames.size(); i++)
	{
    	o << "server_name " << i << ":" << serverNames[i] << std::endl;
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
