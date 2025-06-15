import { compareGoodRequests } from "./compareRequests.mjs"
import { verifyServersAreRunning, exec, printHeader, generateString, COLOR_GREEN, COLOR_RED, COLOR_RESET } from "./utils.mjs"

let	failedTests = [];

async function	runGoodPutTest(target)
{
	const	result = await compareGoodRequests(target, "PUT", generateString(10, 100), {});
	if (result == true)
		console.log(COLOR_GREEN + "[OK] " + COLOR_RESET);
	else
	{
		console.log(COLOR_RED + "[KO] " + COLOR_RESET);
		failedTests.push(target);
	}
	return (result);
}

async function runTests()
{
	printHeader("Allowed Case");
	await runGoodPutTest("/put/allowed/main.html");

	printHeader("Method Not Allowed Case");
	await runGoodPutTest("/put/notAllowed/main.html");

	printHeader("No Right on folder Case");
	await runGoodPutTest("/put/forbidden/main.html");

	printHeader("File Already existing With right Case");
	await runGoodPutTest("/put/mainRight.html");

	printHeader("File Already existing With no right Case");
	await runGoodPutTest("/put/mainNoRight.html");

	printHeader("Creating directory Case");
	await runGoodPutTest("/put/directory/");

	printHeader("Creating directory that already exists, with right and empty Case");
	await runGoodPutTest("/put/alreadyExistingDirEmpty/");

	printHeader("Creating directory that already exists, with right and not empty Case");
	await runGoodPutTest("/put/alreadyExistingDir/");

	printHeader("Creating directory that already exists, with no right and empty Case");
	await runGoodPutTest("/put/alreadyExistingDirEmptyNoRight/");

	printHeader("Creating directory that already exists, with no right and not empty Case");
	await runGoodPutTest("/put/alreadyExistingDirNoRight/");

	printHeader("Existing File");
	await runGoodPutTest("/put/allowed/main.html");

	printHeader("Folder Not Existing");
	await runGoodPutTest("/put/allowed/whereAmI/test.txt");


	exec("cd ../../webserv && ../scripts/requestsTests/cleanPutTest.sh"); // clean webserv
	exec("rm -rf ../../nginx/*"); // clean nginx

	exec("cd ../../webserv && ../scripts/requestsTests/initPutTest.sh"); // setup webserv
	exec("cp -pr ../../webserv/* ../../nginx/"); // setup nginx

	printHeader("Same But With Query String");
	await runGoodPutTest("/put/allowed/main.html?truc=var");
	await runGoodPutTest("/put/notAllowed/main.html?truc=var");
	await runGoodPutTest("/put/forbidden/main.html?truc=var");
	await runGoodPutTest("/put/mainRight.html?truc=var");
	await runGoodPutTest("/put/mainNoRight.html?truc=var");
	await runGoodPutTest("/put/directory/?truc=var");
	await runGoodPutTest("/put/alreadyExistingDirEmpty/?truc=var");
	await runGoodPutTest("/put/alreadyExistingDir/?truc=var");
	await runGoodPutTest("/put/alreadyExistingDirEmptyNoRight/?truc=var");
	await runGoodPutTest("/put/alreadyExistingDirNoRight/?truc=var");
	await runGoodPutTest("/put/allowed/main.html?truc=var");
	await runGoodPutTest("/put/allowed/whereAmI/test.txt?truc=var");

	if (failedTests.length == 0)
		printHeader("Everything Done : " + COLOR_GREEN + "[OK] " + COLOR_RESET);
	else
	{
		printHeader("Everything Done : " + COLOR_RED + "[KO] " + COLOR_RESET);
		console.table(failedTests);
	}
}

async function	run()
{
	verifyServersAreRunning();

	exec("cd ../../webserv && ../scripts/requestsTests/initPutTest.sh"); // setup webserv
	exec("cp -pr ../../webserv/* ../../nginx/"); // setup nginx

	await runTests();

	exec("cd ../../webserv && ../scripts/requestsTests/cleanPutTest.sh"); // clean webserv
	exec("rm -rf ../../nginx/*"); // clean nginx
}

run();
