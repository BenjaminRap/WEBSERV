#include <iostream>
#include "GetRequest.hpp"
# include "ServerConfiguration.hpp"
# include "Route.hpp"
# include "Configuration.hpp"

#define RESPONSE404 "/custom_404.html"
#define SRCS301 "./unitTest/srcs/"
#define CLASSIC "./unitTest/main.html"
#define MAIN200 "./unitTest/fake/main.cpp"
#define FOR403 "Forbidden"
#define MAINCPP "./unitTest/fake/main.cpp"
#define SRCS200 "./unitTest/srcs/index.html"

#define AUTOINDEXBASE "<html>\n<head><title>Index of ./unitTest/autoindex/</title></head>\n<body>\n<h1>Index of./unitTest/autoindex/</h1><hr><pre><a href=\"../\">../</a>\n<a href=\"dir\">dir</a>\n<a href=\"notdir\">notdir</a>\n<a href=\"main.cpp\">main.cpp</a>\n<a href=\"test\">test</a>\n<a href=\"tnul\">tnul</a>\n<a href=\"zebi.js\">zebi.js</a>\n</pre><hr></body>\n</html>"
#define AUTOINDEXDIR "<html>\n<head><title>Index of ./unitTest/autoindex/dir/</title></head>\n<body>\n<h1>Index of./unitTest/autoindex/dir/</h1><hr><pre><a href=\"../\">../</a>\n<a href=\"..\">..</a>\n</pre><hr></body>\n</html>"


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
		std::cout << BMAG << "Request : "<< BCYN << test << tab << BGRN << "OK" << CRESET << std::endl;
	else if (a.code != code || a.file != response)
		std::cout << BMAG << "Request : "<< BCYN << test << tab << BRED << "KO : " << a.code << " | " << a.file << CRESET <<std::endl;
}

void	unitsTest(const ServerConfiguration& config)
{
	std::cout << BMAG << "|-----------------------------------|" << CRESET << std::endl;
	std::cout << BYEL << "Please Make sure the Following directory are present :\n\t- unitTest" << CRESET << std::endl;
	std::cout << BMAG << "|-----------------------------------|" << CRESET << std::endl;
	std::cout << BBLU << "\t Classic Case" << CRESET << std::endl;
	std::cout << BMAG << "|-----------------------------------|" << CRESET << std::endl;
	makeTest("/main.html", 200, CLASSIC, "\t\t\t\t\t", config);
	makeTest("/fake/main.cpp", 200, MAINCPP, "\t\t\t\t", config);
	makeTest("/fake/../main.html", 200, CLASSIC, "\t\t\t\t", config);
	makeTest("/fake/../../../../../../../../main.html", 200, CLASSIC, "\t", config);
	makeTest("/../main.html", 200, CLASSIC, "\t\t\t\t\t", config);
	std::cout << BMAG << "|-----------------------------------|" << CRESET << std::endl;
	std::cout << BBLU << "\t Index Case" << CRESET << std::endl;
	std::cout << BMAG << "|-----------------------------------|" << CRESET << std::endl;
	makeTest("/unitTest/", 200, CLASSIC, "\t\t\t\t\t", config);
	makeTest("/", 200, CLASSIC, "\t\t\t\t\t\t", config);
	makeTest("/unitTest/srcs/", 200, SRCS200, "\t\t\t\t", config);
	std::cout << BMAG << "|-----------------------------------|" << CRESET << std::endl;
	std::cout << BBLU << "\t Redirection Case" << CRESET << std::endl;
	std::cout << BMAG << "|-----------------------------------|" << CRESET << std::endl;
	makeTest("/srcs", 301, "./unitTest/srcs/", "\t\t\t\t\t\t", config);
	makeTest("/redirect-me", 301, "/", "\t\t\t\t\t", config);
	makeTest("", 301, "./unitTest/", "\t\t\t\t\t\t", config);
	std::cout << BMAG << "|-----------------------------------|" << CRESET << std::endl;
	std::cout << BBLU << "\t 403 Case" << CRESET << std::endl;
	std::cout << BMAG << "|-----------------------------------|" << CRESET << std::endl;
	makeTest("/fake/", 403, FOR403, "\t\t\t\t\t", config);
	makeTest("/nonono/", 403, FOR403, "\t\t\t\t\t", config);
	std::cout << BMAG << "|-----------------------------------|" << CRESET << std::endl;
	std::cout << BBLU << "\t 405 Case" << CRESET << std::endl;
	std::cout << BMAG << "|-----------------------------------|" << CRESET << std::endl;
	makeTest("/unitTest/upload/", 405, "Method Not Allowed", "\t\t\t\t", config);
	std::cout << BMAG << "|-----------------------------------|" << CRESET << std::endl;
	std::cout << BBLU << "\t 404 Case" << CRESET << std::endl;
	std::cout << BMAG << "|-----------------------------------|" << CRESET << std::endl;
	makeTest("/unitTest/uplo/", 404, RESPONSE404, "\t\t\t\t", config);
	makeTest("gknrk", 404, RESPONSE404, "\t\t\t\t\t\t", config);
	makeTest("/bin/", 404, RESPONSE404, "\t\t\t\t\t\t", config);
	std::cout << BMAG << "|-----------------------------------|" << CRESET << std::endl;
	std::cout << BBLU << "\t Auto Index" << CRESET << std::endl;
	std::cout << BMAG << "|-----------------------------------|" << CRESET << std::endl;
	makeTest("/unitTest/autoindex/", 0, AUTOINDEXBASE, "\t\t\t\t", config);
	makeTest("/unitTest/autoindex/dir/", 0, AUTOINDEXDIR, "\t\t\t", config);
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


