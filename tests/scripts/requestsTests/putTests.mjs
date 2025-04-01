import { compareRequests, verifyServersAreRunning, exec } from "./testServers.mjs"

async function	runPutTest(target)
{
	await compareRequests(target, "GET", null, {});
}

async function runTests()
{
	await runPutTest("/put/allowed/main.html");
	await runPutTest("/put/notAllowed/main.html");
	await runPutTest("/put/forbidden/main.html");
	await runPutTest("/put/mainRight.html");
	await runPutTest("/put/mainNoRight.html");
	await runPutTest("/put/directory/");
	await runPutTest("/put/alreadyExistingDirEmpty/");
	await runPutTest("/put/alreadyExistingDir/");
	await runPutTest("/put/alreadyExistingDirEmptyNoRight/");
	await runPutTest("/put/alreadyExistingDirNoRight/");
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
