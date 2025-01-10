#include "Configuration.hpp"

Configuration::Configuration(void)
{
	return ;
}

Configuration::Configuration(const char *path)
{
	std::string buff;

	ft_readfile(path, buff);
	//std::cout << buff << std::endl;
	parse_file(buff);
	return ;
}

Configuration::~Configuration(void)
{
	return ;
}

void	Configuration::parse_file(std::string &file)
{
	size_t	i = 0;
	size_t	line = 1;

	while (i < file.size())
	{
		skip_wspace(file, i, line);
		if (file[i] == '#')
			skip_line(file, i, line);
		else if (file.substr(i, 6) == "server") // 6 = "server" length
		{
			i += 6;
			skip_wspace(file, i , line);
			if (file[i] != '{')
				throw (UnexpectedKeyWordException(line, file.substr(i, file.find_first_of(WSPACE, i))));
			else
				parse_server(file, i, line);
		}
		else
			throw (UnexpectedKeyWordException(line, file.substr(i, file.find_first_of(WSPACE, i))));
	}
}

void	Configuration::parse_server(std::string &file, size_t &i, size_t &line)
{
	uint32_t								host = 0;
	uint16_t								port = 0;
	// std::vector<std::string>				serverNames;
	// std::map<unsigned short, std::string>	errorPages;
	// size_t									maxClientBodySize;
	// std::vector<Route>						routes;

	i++;
	while (i < file.size() && file[i] != '}')
	{
		skip_wspace(file, i, line);
		if (file[i] == '#')
			skip_line(file, i, line);
		else if (file.substr(i, 6) == "listen") // 6 = "listen" length
		{
			if (host || port)
				throw (MultipleDefinitionException(line, "host/port"));
			i += 6;
			skip_wspace(file, i , line);
			parse_ip(file, i, line, host);
			parse_port(file, i, line, port);
			skip_wspace(file, i , line);
			std::cout << "host:" << (host >> 24) << ((host << 8) >> 24) << ((host << 16) >> 24) << ((host << 24) >> 24) << std::endl;
			std::cout << "port:" << port << std::endl;
			if (file[i] != ';')
				throw (MissingSemiColonException(line));
			i++;
		}
		else if (file.substr(i, 20) == "client_max_body_size")
		{
			i += 20;
			skip_wspace(file, i , line);
		}
		i++;
	}
}

void	Configuration::skip_line(std::string &file, size_t &i, size_t &line)
{
	i = file.find('\n', i);
	if (i != file.size())
		line++;
	i++;
}

void	Configuration::skip_wspace(std::string &file, size_t &i, size_t &line)
{
	while (std::isspace(static_cast<unsigned char>(file[i])))
	{
		if (file[i] == '\n')
			line++;
		i++;
	}
}

short	Configuration::real_atoi(std::string &file, size_t &i, size_t &line, short max, short len)
{
	short	nb = 0;

	while (len-- > 0 && std::isdigit(file[i]))
	{
		nb = nb * 10 + (file[i] - '0');
		i++;
		len++;
	}
	if (nb > max || nb < 0)
		throw (WrongIpFormatException(line));
	return (nb);
}

void	Configuration::parse_ip(std::string &file, size_t &i, size_t &line, uint32_t &host)
{
	host = real_atoi(file, i, line, 255, 3) << 24;
	if (file[i] != '.')
		throw (WrongIpFormatException(line));
	i++;
	host = host | real_atoi(file, i, line, 255, 3) << 16;
	if (file[i] != '.')
		throw (WrongIpFormatException(line));
	i++;
	host = host | real_atoi(file, i, line, 255, 3) << 8;
	if (file[i] != '.')
		throw (WrongIpFormatException(line));
	i++;
	host = host | real_atoi(file, i, line, 255, 3);
}

void	Configuration::parse_port(std::string &file, size_t &i, size_t &line, uint16_t &port)
{
	if (file[i] != ':')
		throw (WrongIpFormatException(line));
	i++;
	port = real_atoi(file, i, line, 9999, 4);
}
