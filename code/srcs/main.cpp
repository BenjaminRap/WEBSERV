#include <iostream>
#include "GetRequest.hpp"
# include "ServerConfiguration.hpp"
# include "Route.hpp"
# include "Configuration.hpp"

#define RESPONSE404 "404/page"
#define SRCS301 "./unitTest/srcs/"
#define CLASSIC "./unitTest/main.html"
#define MAIN200 "./unitTest/fake/main.cpp"
#define FOR403 "Forbidden"
#define MAINCPP "./unitTest/fake/main.cpp"

/*
 * Classic





makeTest("/xasdw", 404, RESPONSE404, "\t\t", config);
makeTest("/srcs", 301, SRCS301, "\t\t\t", config);
makeTest("/srcs/", 200, INDEX200, "\t\t", config);
makeTest("/srcs/index.html", 200, INDEX200, "\t", config);
makeTest("/../../../../../../", 0, HTML_CONTENT, "\t", config);
makeTest("/fake/", 200, MAIN200, "\t\t", config);
makeTest("/..//.././../", 0, HTML_CONTENTW, "\t\t", config);
makeTest("/nonono/", 403, FOR403, "\t\t", config);
*/

#define BBLK "\e[1;3;30m"
#define BRED "\e[1;3;31m"
#define BGRN "\e[1;3;32m"
#define BYEL "\e[1;3;33m"
#define BBLU "\e[1;3;34m"
#define BMAG "\e[1;3;35m"
#define BCYN "\e[1;3;36m"
#define BWHT "\e[1;3;37m"
#define CRESET "\e[0m"


void	makeTest(const std::string& test, int code, const std::string& response, const std::string& tab, ServerConfiguration config)
{
	GetRequest a(test, config);

	if (a.code == code && a.file == response)
		std::cout << BMAG << "Request : "<< BCYN << test << tab << BGRN << "GOOD" << CRESET << std::endl;
	else if (a.code != code || a.file != response)
		std::cout << BMAG << "Request : "<< BCYN << test << tab << BRED << "FALSE " << a.file << CRESET <<std::endl;
}

void	unitsTest(const ServerConfiguration& config)
{
	std::cout << BMAG << "|-----------------------------------|" << CRESET << std::endl;
	std::cout << BYEL << "Please Make sure the Following directory are present :\n\t- unitTest" << CRESET << std::endl;
	std::cout << BMAG << "|-----------------------------------|" << CRESET << std::endl;
	std::cout << CRESET << std::endl;
	std::cout << BMAG << "|-----------------------------------|" << CRESET << std::endl;
	(void)config;
	makeTest("/", 200, CLASSIC, "\t\t\t", config);
	makeTest("/unitTest/fake/main.cpp", 200, MAINCPP, "\t\t\t", config);
	makeTest("/unitTest/fake/../", 200, CLASSIC, "\t\t\t", config);
	makeTest("/unitTest/fake/../../../../../../../../", 200, CLASSIC, "\t\t\t", config);
}

int	main(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	Configuration	config(argv[1]);

	if (argc == 2)
		unitsTest(config.getServerConfiguration(0));
	else if (argc == 3)
	{
		GetRequest a(argv[2], config.getServerConfiguration(0));
		std::cout << BMAG << "|-----------------------------------|" << CRESET << std::endl;
		std::cout << BMAG << "Request : "<< BCYN << argv[2] << "\t" << CRESET << std::endl;
		std::cout << BMAG << "|-----------------------------------|" << CRESET << std::endl;
		std::cout << CRESET << std::endl;
		std::cout << BMAG << "Code : "<< BWHT << a.code << CRESET << std::endl;
		std::cout << BMAG << "File : "<< BWHT << a.file << CRESET << std::endl;
		std::cout << std::endl;
		std::cout << BMAG << "|-----------------------------------|" << CRESET << std::endl;
	}
	return (0);
}


