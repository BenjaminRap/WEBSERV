import { compareRequests, verifyServersAreRunning, exec, printHeader } from "./testServers.mjs"

async function	runPutTest(header, target)
{
	printHeader(header);
	await compareRequests(target, "GET", null, {});
}

async function runTests()
{
	await runPutTest("Allowed Case", "/put/allowed/main.html");
	await runPutTest("Method Not Allowed Case", "/put/notAllowed/main.html");
	await runPutTest("No Right on folder Case", "/put/forbidden/main.html");
	await runPutTest("File Already existing With right Case", "/put/mainRight.html");
	await runPutTest("File Already existing With no right Case", "/put/mainNoRight.html");
	await runPutTest("Creating directory Case", "/put/directory/");
	await runPutTest("Creating directory that already exists, with right and empty Case", "/put/alreadyExistingDirEmpty/");
	await runPutTest("Creating directory that already exists, with right and not empty Case", "/put/alreadyExistingDir/");
	await runPutTest("Creating directory that already exists, with no right and empty Case", "/put/alreadyExistingDirEmptyNoRight/");
	await runPutTest("Creating directory that already exists, with no right and not empty Case", "/put/alreadyExistingDirNoRight/");
}

async function	run()
{
	verifyServersAreRunning();

	exec("mkdir -p ../../../code/unitTest && cd ../../../code/unitTest && ../../tests/scripts/requestsTests/initPutTest.sh"); // setup webserv
	exec("cd ../../website && ../scripts/requestsTests/initPutTest.sh"); // setup nginx

	await runTests();

	exec("cd ../../../code/unitTest && ../../tests/scripts/requestsTests/cleanPutTest.sh"); // clean webserv
	exec("cd ../../website && ../scripts/requestsTests/cleanPutTest.sh"); // clean nginx
}

run();
