#ifndef	PARSING_HPP
# define PARSING_HPP

# include <netinet/in.h>  // for in_port_t, in_addr_t
# include <stdint.h>      // for uint8_t
# include <cstring>       // for size_t, memcmp
# include <map>           // for map, operator<
# include <string>        // for string, basic_string, operator<
# include <vector>        // for vector, operator<

# include "EMethods.hpp"  // for EMethods

class Configuration;
class Route;
class ServerConfiguration;
struct SRedirection;

# define WSPACE "\t\n\v\f\r "
# define SEP_WSPACE "\t\n\v\f\r ;"
# define SEP_WSPACE_ARG "\t\n\v\f\r ;{}"
# define DIGITS "0123456789"

typedef struct ipv6_s {
    uint8_t ipv6[16];

	bool operator<(const ipv6_s& other) const {
        return std::memcmp(this->ipv6, other.ipv6, sizeof(uint8_t [16])) < 0;
	}

    bool operator==(const ipv6_s& other) const {
        return std::memcmp(this->ipv6, other.ipv6, sizeof(uint8_t [16])) == 0;
	}

	bool operator==(const uint8_t (&other)[16]) const {
        return std::memcmp(this->ipv6, other, sizeof(uint8_t [16])) == 0;
	}

    uint8_t const (&getRef(void) const)[16] {
        return ipv6; 
    }

}	ipv6_t;

typedef struct ip_s
{
	std::vector<std::string>		unix_adrr;
	std::map<in_addr_t, in_port_t>	ipv4;
	std::map<ipv6_t, in_port_t>		ipv6;

	bool	operator<(const ip_s &other) const
	{
		return (this->unix_adrr < other.unix_adrr || this->ipv4 < other.ipv4 || this->ipv6 < other.ipv6);
	}
}	ip_t;

void	parseFile(Configuration &config, std::string &file);
void	parseServer(std::map<ip_t, std::vector<ServerConfiguration> > &conf, std::string &file, size_t &i, size_t &line);
void	skipLine(std::string &file, size_t &i, size_t &line);
void	skipWSpace(std::string &file, size_t &i, size_t &line);
short	realAtoi(std::string &file, size_t &i, size_t &line, short max, short len);
uint8_t	hexToInt(std::string &file, size_t &i, size_t &line);
void	parseIpv4(std::string &file, size_t &i, size_t &line, std::map<in_addr_t, in_port_t> &ip);
void	parseHost(std::string &file, size_t &i, size_t &line, ip_t &ip);
void	parseIpv6(std::string &file, size_t &i, size_t &line, std::map<ipv6_t, in_port_t> &ip);
void	parseIpUnix(std::string &file, size_t &i, size_t &line, std::vector<std::string> &ip);
void	parsePort(std::string &file, size_t &i, size_t &line, in_port_t &port);
void	parseMaxClientBodySize(std::string &file, size_t &i, size_t &line, size_t &maxClientBodySize);
void	parseServerName(std::string &file, size_t &i, size_t &line, std::vector<std::string> &serverNames);
void	parseErrorPages(std::string &file, size_t &i, size_t &line, std::map<unsigned short, std::string> &errorPages);
void	parseRoute(std::string &file, size_t &i, size_t &line, std::map<std::string, Route> &routes);
void	parseRoot(std::string &file, size_t &i, size_t &line, std::string &root);
void	parseRouteAutoIndex(std::string &file, size_t &i, size_t &line, bool &auto_index);
void	parseRouteIndex(std::string &file, size_t &i, size_t &line, std::vector<std::string> &index);
void	parseRouteAcceptedMethod(std::string &file, size_t &i, size_t &line, std::vector<EMethods> &acceptedMethods);
void	parseRouteRedirection(std::string &file, size_t &i, size_t &line, SRedirection &redirection);
void	parseRouteCgiInterpreter(std::string &file, size_t &i, size_t &line, std::string &cgiInterpreter);
void	parseRouteCgiFileExtension(std::string &file, size_t &i, size_t &line, std::string &cgiFileExtention);
void	parseAddHeader(std::string &file, size_t &i, size_t &line, std::map< std::string, std::pair<std::string, bool> > &addHeader);
void	readfile(const char *path, std::string &buff);
void	insertHost
(
	std::map<ip_t, std::vector<ServerConfiguration> > &conf,
	std::vector<std::string> &serverNames,
	std::map<unsigned short, std::string> &errorPages,
	size_t maxClientBodySize,
	std::vector<EMethods>& acceptedMethods,
	std::map<std::string, Route> &routes,
	std::string &root,
	ip_t &ip,
	std::vector<std::string> &index,
	std::map< std::string, std::pair<std::string, bool> > &addHeader,
	std::string& cgiFileExtension,
	std::string& cgiInterpreter
);

#endif
