#include "ServerConfiguration.hpp"

ServerConfiguration::ServerConfiguration(	uint32_t host, \
											uint16_t port, \
											std::vector<std::string> serverNames, \
											std::map<unsigned short, \
											std::string> errorPages, \
											size_t maxClientBodySize, \
											std::map<std::string, Route> routes	) : \
											host(host), \
											port(port), \
											serverNames(serverNames), \
											errorPages(errorPages), \
											maxClientBodySize(maxClientBodySize), \
											routes(routes)
{
	return ;
}

ServerConfiguration::~ServerConfiguration(void)
{
	return ;
}

const uint32_t					&ServerConfiguration::getHost(void) const
{
	return (this->host);
}

const uint16_t					&ServerConfiguration::getPort(void) const
{
	return (this->port);
}

const std::vector<std::string>	&ServerConfiguration::getServerNames(void) const
{
	return (this->serverNames);
}

const std::string				&ServerConfiguration::getErrorPage(unsigned short error) const
{
    return (this->errorPages.find(error)->second);
}

const size_t					&ServerConfiguration::getMaxClientBodySize(void) const
{
	return (this->maxClientBodySize);
}

const std::map<std::string, Route>	&ServerConfiguration::getRoutes(void)
{
	return (this->routes);
}
