#include "utils.cpp"

void	putTest(void)
{
	printHeader("Allowed Case");
	testServers("", "/put/allowed/main.html", "\t\t\t\t\t");

	printHeader("Method Not Allowed Case");
	testServers("", "/put/notAllowed/main.html", "\t\t\t\t\t");

	printHeader("No Right on folder Case");
	testServers("", "/put/forbidden/main.html", "\t\t\t\t\t");

	printHeader("File Already existing With right Case");
	testServers("", "/put/mainRight.html", "\t\t\t\t\t");

	printHeader("File Already existing With no right Case");
	testServers("", "/put/mainNoRight.html", "\t\t\t\t\t");

	printHeader("Creating directory Case");
	testServers("", "/put/directory/", "\t\t\t\t\t");

	printHeader("Creating directory that already exists, with right and empty Case");
	testServers("", "/put/alreadyExistingDirEmpty/", "\t\t\t\t\t");

	printHeader("Creating directory that already exists, with right and not empty Case");
	testServers("", "/put/alreadyExistingDir/", "\t\t\t\t\t");

	printHeader("Creating directory that already exists, with no right and empty Case");
	testServers("", "/put/alreadyExistingDirEmptyNoRight/",  "\t\t\t\t\t");

	printHeader("Creating directory that already exists, with no right and not empty Case");
	testServers("", "/put/alreadyExistingDirNoRight/", "\t\t\t\t\t");
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

	if (std::system("mkdir -p ../../../code/unitTest && cd ../../../code/unitTest && ../../tests/scripts/requestsTests/initPutTest.sh") != 0  // For our server
		|| std::system("cd ../../website && ../scripts/requestsTests/initPutTest.sh") != 0)  // For nginx
	{
		std::cout << "Error executing the put init scripts" << std::endl;
		return (EXIT_FAILURE);
	}

	putTest();


	if (std::system("cd ../../../code/unitTest && ../../tests/scripts/requestsTests/cleanPutTest.sh") != 0 // For our server
		|| std::system("cd ../../website && ../scripts/requestsTests/cleanPutTest.sh") != 0) // For nginx
	{
		std::cout << "Error executing the put cleaning scripts" << std::endl;
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}
