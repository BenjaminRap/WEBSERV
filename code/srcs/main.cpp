#include <iostream>
#include <cstdlib>
#include "GetRequest.hpp"
#include "ServerConfiguration.hpp"
#include "Configuration.hpp"
#include "DeleteRequest.hpp"
#include <cerrno>
#include <cstring>
#include <unistd.h>


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


void	makeTest(const std::string& test, int code, const std::string& response, const std::string& tab, const ServerConfiguration &config)
{
	GetRequest a(test, config);

	if (a.code == code && a.file == response)
		std::cout << BMAG << "Request : "<< BCYN << test << tab << BGRN << "OK" << CRESET << std::endl;
	else if (a.code != code || a.file != response)
		std::cout << BMAG << "Request : "<< BCYN << test << tab << BRED << "KO : " << a.code << " | " << a.file << CRESET <<std::endl;
}

void	makeDelete(const std::string &desc, const std::string& test, int code, const std::string& response, const std::string& tab, const ServerConfiguration &config)
{
	DeleteRequest a(test, config);

	if (a.code == code && a.file == response)
		std::cout << BMAG << "Test : "<< BCYN << desc << tab << BGRN << "OK : " << code << CRESET << std::endl;
	else if (a.code != code || a.file != response)
		std::cout << BMAG << "Test : "<< BCYN << desc << tab << BRED << "KO : " << "request : " << test << " response : " << a.code << " | " << a.file << CRESET <<std::endl;
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
	ss >> statusText;
	close(tube[0]);
	close(tube[1]);
	return (std::pair<int, std::string>(status, statusText));
}

void	testDeleteRequest(const std::string &desc, const std::string &url, const std::string &tab, const ServerConfiguration &config)
{
	std::pair<int, std::string>	nginxResult;
	
	nginxResult = askNginx(url, "GET");
	makeDelete(desc, url, nginxResult.first, nginxResult.second, tab, config);
}

void	unitsTest(const ServerConfiguration &config)
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
	makeTest("", 400, "Bad Request", "\t\t\t\t\t\t", config);
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
	makeTest("/gknrk", 404, RESPONSE404, "\t\t\t\t\t", config);
	makeTest("/bin/", 404, RESPONSE404, "\t\t\t\t\t\t", config);
	std::cout << BMAG << "|-----------------------------------|" << CRESET << std::endl;
	std::cout << BBLU << "\t Auto Index" << CRESET << std::endl;
	std::cout << BMAG << "|-----------------------------------|" << CRESET << std::endl;
	makeTest("/unitTest/autoindex/", 0, AUTOINDEXBASE, "\t\t\t\t", config);
	makeTest("/unitTest/autoindex/dir/", 0, AUTOINDEXDIR, "\t\t\t", config);
}

void	deleteTest(const ServerConfiguration &config)
{
	std::cout << BMAG << "|-----------------------------------|" << CRESET << std::endl;
	std::cout << BYEL << "Please Make sure the Following directory are present :\n\t- unitTest \n\t- test.sh\n\t- removeTest.sh" << CRESET << std::endl;
	std::cout << BMAG << "|-----------------------------------|" << CRESET << std::endl;
	std::cout << BBLU << "\t File Case" << CRESET << std::endl;
	std::cout << BMAG << "|-----------------------------------|" << CRESET << std::endl;
	testDeleteRequest("Normal Case", "/temp/full/classic", "\t\t\t\t\t", config);
	testDeleteRequest("Normal Case (\"../\" in URL)","/temp/full/../../temp/full/noback", "\t\t\t", config);
	testDeleteRequest("No Perm file","/temp/full/noperms", "\t\t\t\t\t", config);
	testDeleteRequest("No Perm directory of file","/temp/cant/tryme", "\t\t\t", config);
	testDeleteRequest("Only Read Perm directory of file","/temp/readme/deleteme", "\t\t\t", config);
	testDeleteRequest("Not Found","/temp/emptwswdy", "\t\t\t\t\t", config);
	std::cout << BMAG << "|-----------------------------------|" << CRESET << std::endl;
	std::cout << BBLU << "\t Directory Case" << CRESET << std::endl;
	std::cout << BMAG << "|-----------------------------------|" << CRESET << std::endl;
	testDeleteRequest("Url not end by \"/\"","/temp/folder/empty", "\t\t\t\t", config);
	testDeleteRequest("Empty Directory","/temp/folder/empty/", "\t\t\t\t\t", config);
	testDeleteRequest("Normal Case","/temp/folder/classic/", "\t\t\t\t\t", config);
	testDeleteRequest("No Perms for parent dir","/temp/folder/nopermspa/", "\t\t\t\t", config);
	testDeleteRequest("No Perms for the dir do del","/temp/folder/noperms/", "\t\t\t", config);
	testDeleteRequest("Dir in Dir (But have no perms)","/temp/folder/dire/", "\t\t\t", config);
	testDeleteRequest("Dir in Dir (Have no perms but empty)","/temp/folder/dire2/", "\t\t", config);
	testDeleteRequest("Dir in Dir (Read Only but empty)","/temp/folder/dire3/", "\t\t\t", config);
	testDeleteRequest("Normal Case ++++++","/temp/folder/dire4/", "\t\t\t\t", config);
}

int	main(int argc, char **argv)
{
	Configuration	config(argv[1]);

	if (argc == 2)
	{
		std::system("./test.sh");
		deleteTest(config.getServerConfiguration(0));
		std::system("./removeTest.sh");
	}
	else if (argc == 3)
	{
		DeleteRequest a(argv[2], config.getServerConfiguration(0));
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


