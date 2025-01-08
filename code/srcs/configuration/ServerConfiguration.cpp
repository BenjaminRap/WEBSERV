#include "ServerConfiguration.hpp"

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

std::vector<Route>				&ServerConfiguration::getRoutes(void)
{
	return (this->routes);
}
