#ifndef	PARSING_HPP
# define PARSING_HPP

# include <string>
# include <iostream>
# include <map>
# include <cstdlib>
# include <fstream>
# include <cstring>
# include <utility>
# include <netinet/in.h>
# include "Configuration.hpp"
# include "exception.hpp"

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

void	parse_file(Configuration &config, std::string &file);
void	parse_server(std::map<ip_t, std::vector<ServerConfiguration> > &conf, std::string &file, size_t &i, size_t &line);
void	skip_line(std::string &file, size_t &i, size_t &line);
void	skip_wspace(std::string &file, size_t &i, size_t &line);
short	real_atoi(std::string &file, size_t &i, size_t &line, short max, short len);
uint8_t	ft_hextoint(std::string &file, size_t &i, size_t &line);
void	parse_ipv4(std::string &file, size_t &i, size_t &line, std::map<in_addr_t, in_port_t> &ip);
void	parse_host(std::string &file, size_t &i, size_t &line, ip_t &ip);
void	parse_ipv6(std::string &file, size_t &i, size_t &line, std::map<ipv6_t, in_port_t> &ip);
void	parse_ip_unix(std::string &file, size_t &i, size_t &line, std::vector<std::string> &ip);
void	parse_port(std::string &file, size_t &i, size_t &line, in_port_t &port);
void	parse_maxClientBodySize(std::string &file, size_t &i, size_t &line, size_t &maxClientBodySize);
void	parse_servername(std::string &file, size_t &i, size_t &line, std::vector<std::string> &serverNames);
void	parse_errorpages(std::string &file, size_t &i, size_t &line, std::map<unsigned short, std::string> &errorPages);
void	parse_route(std::string &file, size_t &i, size_t &line, std::map<std::string, Route> &routes);
void	parse_root(std::string &file, size_t &i, size_t &line, std::string &root);
void	parse_route_autoindex(std::string &file, size_t &i, size_t &line, bool &auto_index);
void	parse_route_index(std::string &file, size_t &i, size_t &line, std::vector<std::string> &index);
void	parse_route_accepted_method(std::string &file, size_t &i, size_t &line, std::vector<EMethods> &acceptedMethods);
void	parse_route_redirection(std::string &file, size_t &i, size_t &line, SRedirection &redirection);
void	parse_route_uploads(std::string &file, size_t &i, size_t &line, bool &acceptUploads);
void	ft_readfile(const char *path, std::string &buff);
void	insert_host(std::map<ip_t, std::vector<ServerConfiguration> > &conf, std::vector<std::string> \
&serverNames, std::map<unsigned short, std::string> &errorPages, size_t &maxClientBodySize, \
std::map<std::string, Route> &routes, std::string &root, ip_t &ip, std::vector<std::string> &index);

#endif