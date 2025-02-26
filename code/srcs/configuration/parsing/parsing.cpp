#include "parsing.hpp"
#include "error_pages.hpp"

void	parse_file(Configuration &config, std::string &file)
{
	size_t	i = 0;
	size_t	line = 1;
	std::map<ip_t, std::vector<ServerConfiguration> >	conf;

	while (i < file.size())
	{
		skip_wspace(file, i, line);
		if (file[i] == '#')
			skip_line(file, i, line);
		else if (file.substr(i, 6) == "server")
		{
			i += 6;
			skip_wspace(file, i , line);
			if (file[i] != '{')
				throw (CustomKeyWordAndLineException("Unexpected keyword", line, file.substr(i, file.find_first_of(WSPACE, i) - i)));
			i++;
			parse_server(conf, file, i, line);
		}
		else if (i == std::string::npos)
			throw (CustomLineException("Unexpected error", line));
		skip_wspace(file, i, line);
	}
	for (std::map<ip_t, std::vector<ServerConfiguration> >::iterator it = conf.begin(); it != conf.end(); ++it)
	{
		if (!it->first.ipv4.empty())
		{
			config.insert(std::make_pair(Host(it->first.ipv4.begin()->first, it->first.ipv4.begin()->second), it->second));
		}
		else if (!it->first.ipv6.empty())
		{
			config.insert(std::make_pair(Host(it->first.ipv6.begin()->first.getRef(), it->first.ipv6.begin()->second), it->second));
		}
		else if (!it->first.unix_adrr.empty())
		{
			config.insert(std::make_pair(Host(it->first.unix_adrr[0]), it->second));
		}
	}
}

void	parse_server(std::map<ip_t, std::vector<ServerConfiguration> > &conf, std::string &file, size_t &i, size_t &line)
{
	std::vector<std::string>				serverNames;
	std::map<unsigned short, std::string>	errorPages;
	size_t									maxClientBodySize = 0;
	std::map<std::string, Route>			routes;
	std::string								root;
	ip_t									ip;

	while (i < file.size() && file[i] != '}')
	{
		skip_wspace(file, i, line);
		if (file[i] == '#')
			skip_line(file, i, line);
		else if (!file.compare(i, 6, "listen"))
		{
			i += 6;
			parse_host(file, i, line, ip);
		}
		else if (!file.compare(i, 20, "client_max_body_size"))
		{
			i += 20;
			parse_maxClientBodySize(file, i, line, maxClientBodySize);
		}
		else if (!file.compare(i, 11, "server_name"))
		{
			i += 11;
			parse_servername(file, i, line, serverNames);
		}
		else if (!file.compare(i, 10, "error_page"))
		{
			i += 10;
			parse_errorpages(file, i, line, errorPages);
		}
		else if (!file.compare(i, 8, "location"))
		{
			i += 8;
			parse_route(file, i, line, routes);
		}
		else if (!file.compare(i, 4, "root"))
		{
			i += 4;
			parse_root(file, i, line, root);
		}
		else if (file[i] != '}')
			throw (CustomKeyWordAndLineException("Unexpected keyword", line, file.substr(i, file.find_first_of(WSPACE, i) - i)));
		else if (i == std::string::npos)
			throw (CustomLineException("Unexpected error", line));
		skip_wspace(file, i, line);
	}
	if (file[i] != '}')
		throw (CustomLineException("Unclosed brace", line));
	i++;
	if (ip.ipv4.empty() && ip.ipv6.empty() && ip.unix_adrr.empty())
		throw (CustomException("Missing host"));
	if (errorPages.find(ERROR_403_INT) == errorPages.end())
		errorPages.insert(std::make_pair(ERROR_403_INT, ERROR_403_STR));
	if (errorPages.find(ERROR_404_INT) == errorPages.end())
		errorPages.insert(std::make_pair(ERROR_404_INT, ERROR_404_STR));
	if (errorPages.find(ERROR_405_INT) == errorPages.end())
		errorPages.insert(std::make_pair(ERROR_405_INT, ERROR_405_STR));
	if (errorPages.find(ERROR_500_INT) == errorPages.end())
		errorPages.insert(std::make_pair(ERROR_500_INT, ERROR_500_STR));
	if (errorPages.find(ERROR_505_INT) == errorPages.end())
		errorPages.insert(std::make_pair(ERROR_505_INT, ERROR_505_STR));
	insert_host(conf, serverNames, errorPages, maxClientBodySize, routes, root, ip);
}

void	parse_maxClientBodySize(std::string &file, size_t &i, size_t &line, size_t &maxClientBodySize)
{
	if (maxClientBodySize)
		throw (CustomKeyWordAndLineException("Multiple definition of", line, "maxClientBodySize"));
	skip_wspace(file, i , line);
	maxClientBodySize = std::atol(file.substr(i, file.find_first_not_of(DIGITS, i) - i).c_str());
	i = file.find_first_not_of(DIGITS, i);
	skip_wspace(file, i , line);
	if (file[i] != ';')
		throw (CustomLineException("Missing semi-colon", line));
	i++;
}

void	parse_servername(std::string &file, size_t &i, size_t &line, std::vector<std::string> &serverNames)
{
	while (i < file.size() && file[i] != ';')
	{
		skip_wspace(file, i , line);
		if (file[i] != ';')
		{
			std::string	name = file.substr(i, file.find_first_of(SEP_WSPACE, i) - i);
			if (name.find('.', 0) == std::string::npos)
				throw (CustomKeyWordAndLineException("Wrong server name", line, name));
			serverNames.push_back(name);
		}
		else
			break ;
		i = file.find_first_of(SEP_WSPACE, i);
		if (i == std::string::npos)
			throw (CustomLineException("Missing semi-colon", line));
	}
	if (file[i] != ';')
		throw (CustomLineException("Missing semi-colon", line));
	i++;
}

void	parse_errorpages(std::string &file, size_t &i, size_t &line, std::map<unsigned short, std::string> &errorPages)
{
	std::vector<unsigned short>	errors;
	std::string					error_page;
	
	skip_wspace(file, i, line);
	while (std::isdigit(file[i]))
	{
		errors.push_back(real_atoi(file, i, line, 999, 3));
		skip_wspace(file, i, line);
	}
	if (file[i] != '/')
		throw (CustomLineException("Missing error pages", line));
	error_page = file.substr(i, file.find_first_of(SEP_WSPACE, i) - i);
	i = file.find_first_of(SEP_WSPACE, i);
	skip_wspace(file, i, line);
	if (file[i] != ';')
		throw (CustomLineException("Missing semi-colon", line));
	i++;
	for (std::vector<unsigned short>::iterator it = errors.begin(); it != errors.end(); it++)
	{
		errorPages.insert(std::make_pair(*it, error_page));
	}
}

void	parse_root(std::string &file, size_t &i, size_t &line, std::string &root)
{
	if (!root.empty())
		throw (CustomLineException("Multiple definition of root", line));
	skip_wspace(file, i, line);
	root = file.substr(i, file.find_first_of(SEP_WSPACE, i) - i);
	i = file.find_first_of(SEP_WSPACE, i);
	skip_wspace(file, i, line);
	if (file[i] != ';')
		throw (CustomLineException("Missing semi-colon", line));
	i++;
}
