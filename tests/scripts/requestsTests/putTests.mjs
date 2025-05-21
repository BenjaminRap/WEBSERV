import { compareGoodRequests } from "./compareRequests.mjs"
import { verifyServersAreRunning, exec, printHeader, generateString, COLOR_GREEN, COLOR_RED, COLOR_RESET } from "./utils.mjs"

const	printOK = true;

async function	runGoodPutTest(target)
{
	const	result = await compareGoodRequests(target, "PUT", generateString(10, 100), {}, printOK);
	if (result == true)
		console.log(COLOR_GREEN + "[OK] " + COLOR_RESET);
	else
		console.log(COLOR_RED + "[KO] " + COLOR_RESET);
	return (result);
}

async function runTests()
{
	let	succeed = true;

	printHeader("Allowed Case");
	succeed = await runGoodPutTest("/put/allowed/main.html") && succeed;

	printHeader("Method Not Allowed Case");
	succeed = await runGoodPutTest("/put/notAllowed/main.html") && succeed;

	printHeader("No Right on folder Case");
	succeed = await runGoodPutTest("/put/forbidden/main.html") && succeed;

	printHeader("File Already existing With right Case");
	succeed = await runGoodPutTest("/put/mainRight.html") && succeed;

	printHeader("File Already existing With no right Case");
	succeed = await runGoodPutTest("/put/mainNoRight.html") && succeed;

	printHeader("Creating directory Case");
	succeed = await runGoodPutTest("/put/directory/") && succeed;

	printHeader("Creating directory that already exists, with right and empty Case");
	succeed = await runGoodPutTest("/put/alreadyExistingDirEmpty/") && succeed;

	printHeader("Creating directory that already exists, with right and not empty Case");
	succeed = await runGoodPutTest("/put/alreadyExistingDir/") && succeed;

	printHeader("Creating directory that already exists, with no right and empty Case");
	succeed = await runGoodPutTest("/put/alreadyExistingDirEmptyNoRight/") && succeed;

	printHeader("Creating directory that already exists, with no right and not empty Case");
	succeed = await runGoodPutTest("/put/alreadyExistingDirNoRight/") && succeed;

	printHeader("Existing File");
	succeed = await runGoodPutTest("/put/allowed/main.html") && succeed;

	printHeader("Folder Not Existing");
	succeed = await runGoodPutTest("/put/allowed/whereAmI/test.txt") && succeed;

	if (succeed)
		printHeader("Everything Done : " + COLOR_GREEN + "[OK] " + COLOR_RESET);
	else
		printHeader("Everything Done : " + COLOR_RED + "[KO] " + COLOR_RESET);
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
