#include <iostream>
#include <cstdlib>
#include "ServerConfiguration.hpp"
#include "Configuration.hpp"
#include "DeleteRequest.hpp"
#include "PutRequest.hpp"
#include <cerrno>
#include <cstring>
#include <unistd.h>
#include "parsing.hpp"

#include "utils.cpp"

void	makeDelete(const std::string &desc, const std::string& test, int code, const std::string& response, const std::string& tab, const ServerConfiguration &config)
{
	DeleteRequest a(test, config);

	std::cout << BMAG << "Test : "<< BCYN << desc << BWHT << " | (" << test << ")" << tab;
	if (a.getCode() == code && a.getStatusText() == response)
		std::cout << BGRN << " OK : " << code << CRESET << std::endl;
	else
	{
		std::cout << BRED << " KO : " " response : " << a.getCode() << " | " << a.getStatusText() << "\t";
		std::cout << BGRN << " nginx: " << code << " | " << response << CRESET  << std::endl;
	}
}

void	testDeleteRequest(const std::string &desc, const std::string &url, const std::string &tab, const ServerConfiguration &config)
{
	std::pair<int, std::string>	nginxResult;

	nginxResult = askNginx(url, "DELETE");
	makeDelete(desc, url, nginxResult.first, nginxResult.second, tab, config);
}

void	deleteTest(const ServerConfiguration &config)
{
	std::cout << BMAG << "|-----------------------------------|" << CRESET << std::endl;
	std::cout << BBLU << "\t File Case" << CRESET << std::endl;
	std::cout << BMAG << "|-----------------------------------|" << CRESET << std::endl;
	testDeleteRequest("Normal Case", "/delete/full/classic", "\t\t\t\t\t", config);
	testDeleteRequest("Normal Case (\"../\" in URL)","/delete/full/../../delete/full/noback", "\t", config);
	testDeleteRequest("No Perm file","/delete/full/noperms", "\t\t\t\t\t", config);
	testDeleteRequest("No Perm directory of file","/delete/cant/tryme", "\t\t\t\t", config);
	testDeleteRequest("Only Read Perm directory of file","/delete/readme/deleteme", "\t\t", config);
	testDeleteRequest("Not Found","/delete/emptwswdy", "\t\t\t\t\t\t", config);
	std::cout << BMAG << "|-----------------------------------|" << CRESET << std::endl;
	std::cout << BBLU << "\t Directory Case" << CRESET << std::endl;
	std::cout << BMAG << "|-----------------------------------|" << CRESET << std::endl;
	testDeleteRequest("Url not end by \"/\"","/delete/folder/empty", "\t\t\t\t", config);
	testDeleteRequest("Empty Directory","/delete/folder/empty/", "\t\t\t\t", config);
	testDeleteRequest("Normal Case","/delete/folder/classic/", "\t\t\t\t\t", config);
	testDeleteRequest("No Perms for parent dir","/delete/folder/nopermspa/", "\t\t\t", config);
	testDeleteRequest("No Perms for the dir do del","/delete/folder/noperms/", "\t\t\t", config);
	testDeleteRequest("Dir in Dir (But have no perms)","/delete/folder/dire/", "\t\t\t", config);
	testDeleteRequest("Dir in Dir (Have no perms but empty)","/delete/folder/dire2/", "\t\t", config);
	testDeleteRequest("Dir in Dir (Read Only but empty)","/delete/folder/dire3/", "\t\t", config);
	testDeleteRequest("Normal Case ++++++","/delete/folder/dire4/", "\t\t\t\t", config);
}

int	main(int argc, char **argv)
{
	std::string file;
	if (argc == 1)
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

	if (std::system("mkdir -p ./unitTest && cd ./unitTest && ../../tests/scripts/requestsTests/initDeleteTest.sh") != 0  // For our server
		|| std::system("cd ../tests/website && ../scripts/requestsTests/initDeleteTest.sh") != 0)  // For nginx
	{
		std::cout << "Error executing the delete init scripts" << std::endl;
		return (EXIT_FAILURE);
	}

	deleteTest(config.begin()->second[0]);


	if (std::system("cd ./unitTest && ../../tests/scripts/requestsTests/cleanDeleteTest.sh && rmdir ../unitTest") != 0 // For our server
		|| std::system("cd ../tests/website && ../scripts/requestsTests/cleanDeleteTest.sh") != 0) // For nginx
	{
		std::cout << "Error executing the delete cleaning scripts" << std::endl;
		return (EXIT_FAILURE);
	}
	return (0);
}

