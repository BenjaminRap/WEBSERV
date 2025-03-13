#include <iostream>
#include <cstdlib>
#include "Configuration.hpp"
#include "DeleteRequest.hpp"
#include "PutRequest.hpp"
#include <cerrno>
#include <cstring>
#include <unistd.h>
#include "ServerConfiguration.hpp"
#include "parsing.hpp"

#include "utils.cpp"

void	makePut(const std::string& url, int code, const std::string& response, const std::string& tab, const ServerConfiguration &config)
{
	PutRequest a(url, config);

	std::cout << BMAG << "Request : "<< BCYN << url << tab;
	if (a.getCode() == code && ARequestType::getStatusText(a.getCode()) == response)
		std::cout << BGRN << "OK | " << code << " [" << response << "]" << CRESET << std::endl;
	else
	{
		std::cout << BRED << "KO : " << a.getCode() << " | " << ARequestType::getStatusText(a.getCode()) << CRESET << "\t";
		std::cout << BGRN << "nginx : " << code << " | " << response << CRESET << std::endl;
	}
}

void	testPutRequest(const std::string &url, const std::string &tab, const ServerConfiguration &config)
{
	std::pair<int, std::string>	nginxResult;

	nginxResult = askNginx(url, "PUT");
	makePut(url, nginxResult.first, nginxResult.second, tab, config);
}

void	printHeader(const std::string &header)
{
	std::cout << BMAG << "|-----------------------------------|" << CRESET << std::endl;
	std::cout << BBLU << "\t " << header << CRESET << std::endl;
	std::cout << BMAG << "|-----------------------------------|" << CRESET << std::endl;
}

void	putTest(const ServerConfiguration &config)
{
	printHeader("Allowed Case");
	testPutRequest("/put/allowed/main.html", "\t\t\t\t\t", config);

	printHeader("Method Not Allowed Case");
	testPutRequest("/put/notAllowed/main.html", "\t\t\t\t\t", config);

	printHeader("No Right on folder Case");
	testPutRequest("/put/forbidden/main.html", "\t\t\t\t\t", config);

	printHeader("File Already existing With right Case");
	testPutRequest("/put/mainRight.html", "\t\t\t\t\t", config);

	printHeader("File Already existing With no right Case");
	testPutRequest("/put/mainNoRight.html", "\t\t\t\t\t", config);

	printHeader("Creating directory Case");
	testPutRequest("/put/directory/", "\t\t\t\t\t", config);

	printHeader("Creating directory that already exists, with right and empty Case");
	testPutRequest("/put/alreadyExistingDirEmpty/", "\t\t\t\t\t", config);

	printHeader("Creating directory that already exists, with right and not empty Case");
	testPutRequest("/put/alreadyExistingDir/", "\t\t\t\t\t", config);

	printHeader("Creating directory that already exists, with no right and empty Case");
	testPutRequest("/put/alreadyExistingDirEmptyNoRight/",  "\t\t\t\t\t", config);

	printHeader("Creating directory that already exists, with no right and not empty Case");
	testPutRequest("/put/alreadyExistingDirNoRight/", "\t\t\t\t\t", config);
}

int	main(int argc, char **argv)
{
	std::string file;
	if (argc < 2)
	{
		std::cerr << "Not enough arguments, expected the nginxconf." << std::endl;
		return (1);
	}
	Configuration	config;

	ft_readfile(argv[1], file);
	parse_file(config, file);
	
	if (std::system("curl -I localhost:8181 > /dev/null 2>&1") != 0)
	{
		std::cout << "Nginx server not running, exiting" << std::endl;
		return (EXIT_FAILURE);
	}

	if (std::system("mkdir -p ./unitTest && cd ./unitTest && ../../tests/scripts/requestsTests/initPutTest.sh") != 0  // For our server
		|| std::system("cd ../tests/website && ../scripts/requestsTests/initPutTest.sh") != 0)  // For nginx
	{
		std::cout << "Error executing the get init scripts" << std::endl;
		return (EXIT_FAILURE);
	}

	else if (argc == 3)
	{
		PutRequest a(argv[2], config.begin()->second[0]);
		std::cout << BMAG << "|-----------------------------------|" << CRESET << std::endl;
		std::cout << BMAG << "Request : "<< BCYN << argv[2] << "\t" << CRESET << std::endl;
		std::cout << BMAG << "|-----------------------------------|" << CRESET << std::endl;
		std::cout << CRESET << std::endl;
		std::cout << BMAG << "Code : "<< BWHT <<  a.getCode() << CRESET << std::endl;
		std::cout << BMAG << "File : "<< BWHT << a.getFile() << CRESET << std::endl;
		std::cout << BMAG << "FD : "<< BWHT <<  a.getFd() << CRESET << std::endl;
		std::cout << std::endl;
		std::cout << BMAG << "|-----------------------------------|" << CRESET << std::endl;
		close(a.getFd());
	}
	else
		putTest(config.begin()->second[0]);

	if (std::system("cd ./unitTest && ../../tests/scripts/requestsTests/cleanPutTest.sh") != 0) // For our server) != 0) // For nginx
	{
		std::cout << "Error executing the get cleaning scripts" << std::endl;
		return (EXIT_FAILURE);
	}

	if (std::system("cd ../tests/website && ../scripts/requestsTests/cleanPutTest.sh") != 0) // For nginx
	{
		std::cout << "AAAAAAAAAAAAAAAAAAAAAAAAAAError executing the get cleaning scripts" << std::endl;
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}

