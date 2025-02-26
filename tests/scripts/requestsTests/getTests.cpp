#include <iostream>
#include <cstdlib>
#include "GetRequest.hpp"
#include "ServerConfiguration.hpp"
#include "Configuration.hpp"
#include "DeleteRequest.hpp"
#include "PutRequest.hpp"
#include <cerrno>
#include <cstring>
#include <unistd.h>
#include "parsing.hpp"

#include "utils.cpp"

void	makeGet(const std::string& test, int code, const std::string& response, const std::string& tab, const ServerConfiguration &config)
{
	GetRequest a(test, config);

	std::cout << BMAG << "Request : "<< BCYN << test << tab;
	if (a.getCode() == code && a.getStatusText() == response)
		std::cout << BGRN << "OK | " << code << " [" << response << "]" << CRESET << std::endl;
	else
	{
		std::cout << BRED << "KO : " << a.getCode() << " | " << a.getStatusText() << CRESET << "\t";
		std::cout << BGRN << "nginx : " << code << " | " << response << CRESET << std::endl;
	}
}

void	testGetRequest(const std::string &url, const std::string &tab, const ServerConfiguration &config)
{
	std::pair<int, std::string>	nginxResult;

	nginxResult = askNginx(url, "GET");
	makeGet(url, nginxResult.first, nginxResult.second, tab, config);
}

void	getTest(const ServerConfiguration &config)
{
	std::cout << BMAG << "|-----------------------------------|" << CRESET << std::endl;
	std::cout << BYEL << "Please Make sure the Following directory are present :\n\t- unitTest" << CRESET << std::endl;
	std::cout << BMAG << "|-----------------------------------|" << CRESET << std::endl;
	std::cout << BBLU << "\t Classic Case" << CRESET << std::endl;
	std::cout << BMAG << "|-----------------------------------|" << CRESET << std::endl;
	testGetRequest("/get/main.html", "\t\t\t\t\t", config);
	testGetRequest("/get/fake/main.cpp", "\t\t\t\t\t", config);
	testGetRequest("/get/fake/../main.html", "\t\t\t\t", config);
	testGetRequest("/get/fake/../../../../../../../../main.html", "\t\t", config);
	testGetRequest("/get/../main.html", "\t\t\t\t\t", config);
	std::cout << BMAG << "|-----------------------------------|" << CRESET << std::endl;
	std::cout << BBLU << "\t Index Case" << CRESET << std::endl;
	std::cout << BMAG << "|-----------------------------------|" << CRESET << std::endl;
	testGetRequest("/get/srcs/", "\t\t\t\t\t\t", config); // /unitTest/get/srcs/ // 		root /unitTest/;
	std::cout << BMAG << "|-----------------------------------|" << CRESET << std::endl;
	std::cout << BBLU << "\t Redirection Case" << CRESET << std::endl;
	std::cout << BMAG << "|-----------------------------------|" << CRESET << std::endl;
	testGetRequest("/get/srcs", "\t\t\t\t\t\t", config);
	std::cout << BMAG << "|-----------------------------------|" << CRESET << std::endl;
	std::cout << BBLU << "\t 403 Case" << CRESET << std::endl;
	std::cout << BMAG << "|-----------------------------------|" << CRESET << std::endl;
	testGetRequest("/get/fake/", "\t\t\t\t\t\t", config);
	testGetRequest("/get/nonono/", "\t\t\t\t\t\t", config);
	std::cout << BMAG << "|-----------------------------------|" << CRESET << std::endl;
	std::cout << BBLU << "\t 405 Case" << CRESET << std::endl;
	std::cout << BMAG << "|-----------------------------------|" << CRESET << std::endl;
	testGetRequest("/get/405/", "\t\t\t\t\t\t", config);
	std::cout << BMAG << "|-----------------------------------|" << CRESET << std::endl;
	std::cout << BBLU << "\t 404 Case" << CRESET << std::endl;
	std::cout << BMAG << "|-----------------------------------|" << CRESET << std::endl;
	testGetRequest("/unitTest/uplo/", "\t\t\t\t\t", config);
	testGetRequest("/gknrk", "\t\t\t\t\t\t", config);
	testGetRequest("/bin/", "\t\t\t\t\t\t\t", config);
	std::cout << BMAG << "|-----------------------------------|" << CRESET << std::endl;
	std::cout << BBLU << "\t Auto Index" << CRESET << std::endl;
	std::cout << BMAG << "|-----------------------------------|" << CRESET << std::endl;
	testGetRequest("/get/auto/", "\t\t\t\t\t\t", config);
	testGetRequest("/get/auto2/", "\t\t\t\t\t\t", config);
}

int	main(int argc, char **argv)
{
	std::string file;
	if (argc < 2)
	{
		std::cerr << "Not enough arguments, expected the nginxconf and optionnaly an request" << std::endl;
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

	if (std::system("mkdir -p ./unitTest && cd ./unitTest && ../../tests/scripts/requestsTests/getTest.sh") != 0  // For our server
		|| std::system("cd ../tests/website && ../scripts/requestsTests/getTest.sh") != 0)  // For nginx
	{
		std::cout << "Error executing the get init scripts" << std::endl;
		return (EXIT_FAILURE);
	}

		getTest(config.begin()->second[0]);


	if (std::system("cd ./unitTest && ../../tests/scripts/requestsTests/removeGetTest.sh && rmdir ../unitTest") != 0 // For our server
		|| std::system("cd ../tests/website && ../scripts/requestsTests/removeGetTest.sh") != 0) // For nginx
	{
		std::cout << "Error executing the get cleaning scripts" << std::endl;
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}

