#include <iostream>
#include "../includes/GetRequest.hpp"

#define DIR 1
#define FILE 2
#define NF 3

#define BBLK "\e[1;3;30m"
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
	(void)argc;
	(void)argv;

	GetRequest a(argv[1]);

	std::cout << BMAG << "|-----------------------------------|" << CRESET << std::endl;
	std::cout << BYEL << "Request : GET " << BGRN <<argv[1] << "\t|\t" << a.getUrl() << CRESET << std::endl;
	std::cout << std::endl;
	std::cout << BCYN << "Code : " << BWHT << a.code << CRESET<< std::endl;
	std::cout << BCYN << "Response : " << BWHT << a.file << CRESET << std::endl;
	std::cout << BMAG << "|-----------------------------------|" << CRESET << std::endl;

	return (0);
}