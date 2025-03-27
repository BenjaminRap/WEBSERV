#include "utils.cpp"

void	getTest(const std::string &url, const std::string &tabs)
{

	testServers("", url, tabs, "GET");
}

void	getTests(void)
{
	printHeader("\t Classic Case");
	getTest( "/get/main.html", "\t\t\t\t\t");
	getTest( "/get/fake/main.cpp", "\t\t\t\t\t");
	getTest( "/get/fake/../main.html", "\t\t\t\t");
	getTest( "/get/fake/../../../../../../../../main.html", "\t\t");
	getTest( "/get/../main.html", "\t\t\t\t\t");

	printHeader("\t Index Case");
	getTest( "/get/srcs/", "\t\t\t\t\t\t");

	printHeader("\t Redirection Case");
	getTest( "/get/srcs", "\t\t\t\t\t\t");

	printHeader("\t 403 Case");
	getTest( "/get/fake/", "\t\t\t\t\t\t");
	getTest( "/get/nonono/", "\t\t\t\t\t\t");

	printHeader("\t 405 Case");
	getTest( "/get/405/", "\t\t\t\t\t\t");

	printHeader("\t 404 Case");
	getTest( "/unitTest/uplo/", "\t\t\t\t\t");
	getTest( "/gknrk", "\t\t\t\t\t\t");
	getTest( "/bin/", "\t\t\t\t\t\t\t");

	printHeader("\t Auto Index");
	getTest( "/get/auto/", "\t\t\t\t\t\t");
	getTest( "/get/auto2/", "\t\t\t\t\t\t");

	printHeader("\t Folder With File Name");
	getTest( "/get/truc.txt/", "\t\t\t\t\t");

	printHeader("\t Index Specials");
	getTest( "/get/indexIsFolder/", "\t\t\t\t\t");
	getTest( "/get/indexIsSymlink/", "\t\t\t\t\t");
	getTest( "/get/indexIsFolderAndFile/", "\t\t\t\t");
	getTest( "/get/indexIsSymlinkAndFile/", "\t\t\t\t");

	printHeader("\t Fix Url Errors");
	getTest( "/get//main.html", "\t\t\t\t\t");
	getTest( "/get/endWith./index.html", "\t\t\t\t");
}

int	main(void)
{
	if (std::system("curl -I localhost:8181 > /dev/null 2>&1") != 0)
	{
		std::cout << "Nginx server not running, exiting" << std::endl;
		return (EXIT_FAILURE);
	}
	if (std::system("curl -I localhost:8080 > /dev/null 2>&1") != 0)
	{
		std::cout << "Webserv server not running, exiting" << std::endl;
		return (EXIT_FAILURE);
	}

	if (std::system("mkdir -p ../../../code/unitTest && cd ../../../code/unitTest && ../../tests/scripts/requestsTests/initGetTest.sh") != 0  // For our server
		|| std::system("cd ../../website && ../scripts/requestsTests/initGetTest.sh") != 0)  // For nginx
	{
		std::cout << "Error executing the get init scripts" << std::endl;
		return (EXIT_FAILURE);
	}

	getTests();


	if (std::system("cd ../../../code/unitTest && ../../tests/scripts/requestsTests/cleanGetTest.sh") != 0 // For our server
		|| std::system("cd ../../website && ../scripts/requestsTests/cleanGetTest.sh") != 0) // For nginx
	{
		std::cout << "Error executing the get cleaning scripts" << std::endl;
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}
