#include <cstdlib>
#include <sstream>
#include <unistd.h>
#include <iostream>

#define BRED "\e[1;3;31m"
#define BGRN "\e[1;3;32m"
#define BYEL "\e[1;3;33m"
#define BBLU "\e[1;3;34m"
#define BMAG "\e[1;3;35m"
#define BCYN "\e[1;3;36m"
#define BWHT "\e[1;3;37m"
#define CRESET "\e[0m"

std::pair<int, std::string>	askServer(const std::string &host, const std::string &url,const std::string &method)
{
	int					tube[2];
	char				buffer[1024];
	std::stringstream	ss;
	int					status;
	std::string			statusText;

	pipe(tube);
	ss << "node ../makeRequest.js " << host << url << " " << method << " 1>&" << tube[1];
	const std::string	command(ss.str());
	ss.str("");

	system(command.c_str());

	const ssize_t rd = read(tube[0], buffer, sizeof(buffer));

	if (std::string(buffer, rd) == "ERROR")
		throw std::runtime_error("The fetch script crashed");
	ss << std::string(buffer, rd);
	ss >> status;
	std::getline(ss, statusText);
	statusText.erase(0, 1);
	close(tube[0]);
	close(tube[1]);
	return (std::pair<int, std::string>(status, statusText));
}



typedef std::pair<int, std::string>	ServerReturn;

void	makeGet(const std::string &url, const ServerReturn &nginx, const ServerReturn &webserv, const std::string& tab)
{
	std::cout << BMAG << "Request : "<< BCYN << url << tab;
	if (nginx.first == webserv.first && nginx.second == webserv.second)
		std::cout << BGRN << "OK | " << webserv.first << " [" << webserv.second << "]" << CRESET << std::endl;
	else
	{
		std::cout << BRED << "KO : " << webserv.first << " | " << webserv.second << CRESET << "\t";
		std::cout << BGRN << "nginx : " << webserv.first << " | " << nginx.second << CRESET << std::endl;
	}
}

void	testServers(const std::string &url, const std::string &tab)
{
	ServerReturn nginxResult;
	ServerReturn webservResult;

	nginxResult = askServer("http://localhost:8181", url, "GET");
	webservResult = askServer("http://localhost:8080", url, "GET");
	makeGet(url, nginxResult, webservResult, tab);
}
