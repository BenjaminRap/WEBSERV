#include "utils.cpp"

void	getTest(void)
{
	std::cout << BMAG << "|-----------------------------------|" << CRESET << std::endl;
	std::cout << BBLU << "\t Classic Case" << CRESET << std::endl;
	std::cout << BMAG << "|-----------------------------------|" << CRESET << std::endl;
	testServers("/get/main.html", "\t\t\t\t\t");
	testServers("/get/fake/main.cpp", "\t\t\t\t\t");
	testServers("/get/fake/../main.html", "\t\t\t\t");
	testServers("/get/fake/../../../../../../../../main.html", "\t\t");
	testServers("/get/../main.html", "\t\t\t\t\t");
	std::cout << BMAG << "|-----------------------------------|" << CRESET << std::endl;
	std::cout << BBLU << "\t Index Case" << CRESET << std::endl;
	std::cout << BMAG << "|-----------------------------------|" << CRESET << std::endl;
	testServers("/get/srcs/", "\t\t\t\t\t\t");
	std::cout << BMAG << "|-----------------------------------|" << CRESET << std::endl;
	std::cout << BBLU << "\t Redirection Case" << CRESET << std::endl;
	std::cout << BMAG << "|-----------------------------------|" << CRESET << std::endl;
	testServers("/get/srcs", "\t\t\t\t\t\t");
	std::cout << BMAG << "|-----------------------------------|" << CRESET << std::endl;
	std::cout << BBLU << "\t 403 Case" << CRESET << std::endl;
	std::cout << BMAG << "|-----------------------------------|" << CRESET << std::endl;
	testServers("/get/fake/", "\t\t\t\t\t\t");
	testServers("/get/nonono/", "\t\t\t\t\t\t");
	std::cout << BMAG << "|-----------------------------------|" << CRESET << std::endl;
	std::cout << BBLU << "\t 405 Case" << CRESET << std::endl;
	std::cout << BMAG << "|-----------------------------------|" << CRESET << std::endl;
	testServers("/get/405/", "\t\t\t\t\t\t");
	std::cout << BMAG << "|-----------------------------------|" << CRESET << std::endl;
	std::cout << BBLU << "\t 404 Case" << CRESET << std::endl;
	std::cout << BMAG << "|-----------------------------------|" << CRESET << std::endl;
	testServers("/unitTest/uplo/", "\t\t\t\t\t");
	testServers("/gknrk", "\t\t\t\t\t\t");
	testServers("/bin/", "\t\t\t\t\t\t\t");
	std::cout << BMAG << "|-----------------------------------|" << CRESET << std::endl;
	std::cout << BBLU << "\t Auto Index" << CRESET << std::endl;
	std::cout << BMAG << "|-----------------------------------|" << CRESET << std::endl;
	testServers("/get/auto/", "\t\t\t\t\t\t");
	testServers("/get/auto2/", "\t\t\t\t\t\t");
	std::cout << BMAG << "|-----------------------------------|" << CRESET << std::endl;
	std::cout << BBLU << "\t Folder With File Name" << CRESET << std::endl;
	std::cout << BMAG << "|-----------------------------------|" << CRESET << std::endl;
	testServers("/get/truc.txt/", "\t\t\t\t\t");
	std::cout << BMAG << "|-----------------------------------|" << CRESET << std::endl;
	std::cout << BBLU << "\t Index Specials" << CRESET << std::endl;
	std::cout << BMAG << "|-----------------------------------|" << CRESET << std::endl;
	testServers("/get/indexIsFolder/", "\t\t\t\t\t");
	testServers("/get/indexIsSymlink/", "\t\t\t\t\t");
	testServers("/get/indexIsFolderAndFile/", "\t\t\t\t");
	testServers("/get/indexIsSymlinkAndFile/", "\t\t\t\t");
	std::cout << BMAG << "|-----------------------------------|" << CRESET << std::endl;
	std::cout << BBLU << "\t Fix Url Errors" << CRESET << std::endl;
	std::cout << BMAG << "|-----------------------------------|" << CRESET << std::endl;
	testServers("/get//main.html", "\t\t\t\t\t");
	testServers("/get/endWith./index.html", "\t\t\t\t");
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

	getTest();


	if (std::system("cd ../../../code/unitTest && ../../tests/scripts/requestsTests/cleanGetTest.sh") != 0 // For our server
		|| std::system("cd ../../website && ../scripts/requestsTests/cleanGetTest.sh") != 0) // For nginx
	{
		std::cout << "Error executing the get cleaning scripts" << std::endl;
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}

