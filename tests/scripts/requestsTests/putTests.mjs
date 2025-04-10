import { compareGoodRequests } from "./compareRequests.mjs"
import { verifyServersAreRunning, exec, printHeader, generateString } from "./utils.mjs"

function	runGoodPutTest(header, target)
{
	printHeader(header);
	return (compareGoodRequests(target, "PUT", generateString(10, 100), {}));
}

async function runTests()
{
	await runGoodPutTest("Allowed Case", "/put/allowed/main.html");
	await runGoodPutTest("Method Not Allowed Case", "/put/notAllowed/main.html");
	await runGoodPutTest("No Right on folder Case", "/put/forbidden/main.html");
	await runGoodPutTest("File Already existing With right Case", "/put/mainRight.html");
	await runGoodPutTest("File Already existing With no right Case", "/put/mainNoRight.html");
	await runGoodPutTest("Creating directory Case", "/put/directory/");
	await runGoodPutTest("Creating directory that already exists, with right and empty Case", "/put/alreadyExistingDirEmpty/");
	await runGoodPutTest("Creating directory that already exists, with right and not empty Case", "/put/alreadyExistingDir/");
	await runGoodPutTest("Creating directory that already exists, with no right and empty Case", "/put/alreadyExistingDirEmptyNoRight/");
	await runGoodPutTest("Creating directory that already exists, with no right and not empty Case", "/put/alreadyExistingDirNoRight/");
}

async function	run()
{
	verifyServersAreRunning();

	exec("cd ../../webserv && ../scripts/requestsTests/initPutTest.sh"); // setup webserv
	exec("cd ../../nginx && ../scripts/requestsTests/initPutTest.sh"); // setup nginx

	await runTests();

	exec("cd ../../webserv && ../scripts/requestsTests/cleanPutTest.sh"); // clean webserv
	exec("cd ../../nginx && ../scripts/requestsTests/cleanPutTest.sh"); // clean nginx
}

run();
