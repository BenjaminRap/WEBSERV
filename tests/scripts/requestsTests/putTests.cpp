#include <iostream>
#include <cstdlib>
#include "Configuration.hpp"
#include "DeleteRequest.hpp"
#include "PutRequest.hpp"
#include <cerrno>
#include <cstring>
#include <unistd.h>
#include "parsing.hpp"


#define RESPONSE404 "/custom_404.html"
#define CLASSIC "./unitTest/main.html"

#define FOR403 "Forbidden"
#define NOT204 "No Content"
#define INTERNAL500 "Internal Server Error"
#define CON409 "Conflict"

#define MAINCPP "./unitTest/fake/main.cpp"
#define SRCS200 "./unitTest/srcs/index.html"
#define AUTOINDEXBASE "<html>\n<head><title>Index of ./unitTest/autoindex/</title></head>\n<body>\n<h1>Index of./unitTest/autoindex/</h1><hr><pre><a href=\"../\">../</a>\n<a href=\"dir\">dir</a>\n<a href=\"notdir\">notdir</a>\n<a href=\"main.cpp\">main.cpp</a>\n<a href=\"test\">test</a>\n<a href=\"tnul\">tnul</a>\n<a href=\"zebi.js\">zebi.js</a>\n</pre><hr></body>\n</html>"
#define AUTOINDEXDIR "<html>\n<head><title>Index of ./unitTest/autoindex/dir/</title></head>\n<body>\n<h1>Index of./unitTest/autoindex/dir/</h1><hr><pre><a href=\"../\">../</a>\n</pre><hr></body>\n</html>"
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

