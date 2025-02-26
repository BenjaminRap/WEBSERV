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
#include <sstream>
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

void	testDeleteRequest(const std::string &desc, const std::string &url, const std::string &tab, const ServerConfiguration &config)
{
	std::pair<int, std::string>	nginxResult;

	nginxResult = askNginx(url, "DELETE");
	makeDelete(desc, url, nginxResult.first, nginxResult.second, tab, config);
}

void	deleteTest(const ServerConfiguration &config)
{
	std::cout << BMAG << "|-----------------------------------|" << CRESET << std::endl;
	std::cout << BYEL << "Please Make sure the Following directory are present :\n\t- unitTest \n\t- test.sh\n\t- removeTest.sh" << CRESET << std::endl;
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

	if (std::system("mkdir -p ./unitTest && cd ./unitTest && ../../tests/scripts/requestsTests/deleteTest.sh") != 0  // For our server
		|| std::system("cd ../tests/website && ../scripts/requestsTests/deleteTest.sh") != 0)  // For nginx
	{
		std::cout << "Error executing the delete init scripts" << std::endl;
		return (EXIT_FAILURE);
	}

	deleteTest(config.begin()->second[0]);


	if (std::system("cd ./unitTest && ../../tests/scripts/requestsTests/removeDeleteTest.sh && rmdir ../unitTest") != 0 // For our server
		|| std::system("cd ../tests/website && ../scripts/requestsTests/removeDeleteTest.sh") != 0) // For nginx
	{
		std::cout << "Error executing the delete cleaning scripts" << std::endl;
		return (EXIT_FAILURE);
	}
	return (0);
}

