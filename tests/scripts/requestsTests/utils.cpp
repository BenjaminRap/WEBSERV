#include <cstdlib>
#include <stdexcept>
#include <string>
#include <sstream>
#include <unistd.h>

#define BRED "\e[1;3;31m"
#define BGRN "\e[1;3;32m"
#define BYEL "\e[1;3;33m"
#define BBLU "\e[1;3;34m"
#define BMAG "\e[1;3;35m"
#define BCYN "\e[1;3;36m"
#define BWHT "\e[1;3;37m"
#define CRESET "\e[0m"

std::pair<int, std::string>	askNginx(const std::string &url,const std::string &method)
{
	int					tube[2];
	char				buffer[1024];
	std::stringstream	ss;
	int					status;
	std::string			statusText;

	pipe(tube);
	ss << "node ../tests/scripts/makeRequest.js " << url << " " << method << " 1>&" << tube[1];
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
