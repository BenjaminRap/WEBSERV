#include <iostream>
#include <cstdlib>
#include "Configuration.hpp"
#include "DeleteRequest.hpp"
#include "PutRequest.hpp"
#include <cerrno>
#include <cstring>
#include <unistd.h>
#include "parsing.hpp"

#define BRED "\e[1;3;31m"
#define BGRN "\e[1;3;32m"
#define BYEL "\e[1;3;33m"
#define BBLU "\e[1;3;34m"
#define BMAG "\e[1;3;35m"
#define BCYN "\e[1;3;36m"
#define BWHT "\e[1;3;37m"
#define CRESET "\e[0m"

int	main(int argc, char **argv)
{
	std::string file;
	if (argc < 3)
	{
		std::cerr << "Not enough arguments, expected the nginxconf and a request" << std::endl;
		return (1);
	}
	Configuration	config;
	ft_readfile(argv[1], file);
	parse_file(config, file);
	PutRequest a(argv[2], "test.txt", config.begin()->second[0]);
//		GetRequest a(argv[2], config.begin()->second[0]);
	std::cout << BMAG << "|-----------------------------------|" << CRESET << std::endl;
	std::cout << BMAG << "Request : "<< BCYN << argv[2] << "\t" << CRESET << std::endl;
	std::cout << BMAG << "|-----------------------------------|" << CRESET << std::endl;
	std::cout << CRESET << std::endl;
	std::cout << BMAG << "Code : "<< BWHT << a.getCode() << CRESET << std::endl;
	std::cout << BMAG << "File : "<< BWHT << a.getFile() << CRESET << std::endl;
	std::cout << BMAG << "FD : "<< BWHT << a.getFd() << CRESET << std::endl;
	std::cout << std::endl;
	std::cout << BMAG << "|-----------------------------------|" << CRESET << std::endl;
	close(a.getFd());
	return (0);
}

