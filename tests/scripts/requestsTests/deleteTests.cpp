#include "utils.cpp"

void	deleteTest(void)
{
	printHeader("\t File Case");
	testServers("Normal Case", "/delete/full/classic", "\t\t\t\t\t");
	testServers("Normal Case (\"../\" in URL)","/delete/full/../../delete/full/noback", "\t");
	testServers("No Perm file","/delete/full/noperms", "\t\t\t\t\t");
	testServers("No Perm directory of file","/delete/cant/tryme", "\t\t\t\t");
	testServers("Only Read Perm directory of file","/delete/readme/deleteme", "\t\t");
	testServers("Not Found","/delete/emptwswdy", "\t\t\t\t\t\t");

	printHeader("\t Directory Case");
	testServers("Url not end by \"/\"","/delete/folder/empty", "\t\t\t\t");
	testServers("Empty Directory","/delete/folder/empty/", "\t\t\t\t");
	testServers("Normal Case","/delete/folder/classic/", "\t\t\t\t\t");
	testServers("No Perms for parent dir","/delete/folder/nopermspa/", "\t\t\t");
	testServers("No Perms for the dir do del","/delete/folder/noperms/", "\t\t\t");
	testServers("Dir in Dir (But have no perms)","/delete/folder/dire/", "\t\t\t");
	testServers("Dir in Dir (Have no perms but empty)","/delete/folder/dire2/", "\t\t");
	testServers("Dir in Dir (Read Only but empty)","/delete/folder/dire3/", "\t\t");
	testServers("Normal Case ++++++","/delete/folder/dire4/", "\t\t\t\t");
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

	if (std::system("mkdir -p ../../../code/unitTest && cd ../../../code/unitTest && ../../tests/scripts/requestsTests/initDeleteTest.sh") != 0  // For our server
		|| std::system("cd ../../website && ../scripts/requestsTests/initDeleteTest.sh") != 0)  // For nginx
	{
		std::cout << "Error executing the delete init scripts" << std::endl;
		return (EXIT_FAILURE);
	}

	deleteTest();


	if (std::system("cd ../../../code/unitTest && ../../tests/scripts/requestsTests/cleanDeleteTest.sh") != 0 // For our server
		|| std::system("cd ../../website && ../scripts/requestsTests/cleanDeleteTest.sh") != 0) // For nginx
	{
		std::cout << "Error executing the delete cleaning scripts" << std::endl;
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}
