import { compareGoodRequests } from "./compareRequests.mjs"
import { verifyServersAreRunning, exec, printHeader, COLOR_GREEN, COLOR_RED, COLOR_RESET } from "./utils.mjs"

const	printOK = false;

async function	runGoodGetTest(target)
{
	const	result = await compareGoodRequests(target, "GET", null, {}, printOK);
	if (result == true)
		console.log(COLOR_GREEN + "[OK] " + COLOR_RESET);
	else
		console.log(COLOR_RED + "[KO] " + COLOR_RESET);
	return (result);
}

async function runTests()
{
	let	succeed = true;

	printHeader("Classic Case");
	succeed = await runGoodGetTest("/get/main.html") && succeed;
	succeed = await runGoodGetTest("/get/fake/main.cpp") && succeed;
	succeed = await runGoodGetTest("/get/fake/../main.html") && succeed;
	succeed = await runGoodGetTest("/get/fake/../../../../../../../../main.html") && succeed;
	succeed = await runGoodGetTest("/get/../main.html") && succeed;

	printHeader("Index Case");
	succeed = await runGoodGetTest("/get/srcs/") && succeed;

	printHeader("Redirection Case");
	succeed = await runGoodGetTest("/get/srcs") && succeed;

	printHeader("403 Case");
	succeed = await runGoodGetTest("/get/fake/") && succeed;
	succeed = await runGoodGetTest("/get/nonono/") && succeed;

	printHeader("405 Case");
	succeed = await runGoodGetTest("/get/405/") && succeed;

	printHeader("404 Case");
	succeed = await runGoodGetTest("/unitTest/uplo/") && succeed;
	succeed = await runGoodGetTest("/gknrk") && succeed;
	succeed = await runGoodGetTest("/bin/") && succeed;

	printHeader("Auto Index");
	succeed = await runGoodGetTest("/get/auto/") && succeed;
	succeed = await runGoodGetTest("/get/auto2/") && succeed;

	printHeader("Folder With File Name");
	succeed = await runGoodGetTest("/get/truc.txt/") && succeed;

	printHeader("Index Specials");
	succeed = await runGoodGetTest("/get/indexIsFolder/") && succeed;
	succeed = await runGoodGetTest("/get/indexIsSymlink/") && succeed;
	succeed = await runGoodGetTest("/get/indexIsFolderAndFile/") && succeed;
	succeed = await runGoodGetTest("/get/indexIsSymlinkAndFile/") && succeed;

	printHeader("Fix Url Errors");
	succeed = await runGoodGetTest("/get//main.html") && succeed;
	succeed = await runGoodGetTest("/get/endWith./index.html") && succeed;

	if (succeed)
		printHeader("Everything Done : " + COLOR_GREEN + "[OK] " + COLOR_RESET);
	else
		printHeader("Everything Done : " + COLOR_RED + "[KO] " + COLOR_RESET);
}

async function	run()
{
	verifyServersAreRunning();

	exec("cd ../../webserv && ../scripts/requestsTests/initGetTest.sh"); // setup webserv
	exec("cd ../../nginx && ../scripts/requestsTests/initGetTest.sh"); // setup nginx

	await runTests();

	exec("cd ../../webserv && ../scripts/requestsTests/cleanGetTest.sh"); // clean webserv
	exec("cd ../../nginx && ../scripts/requestsTests/cleanGetTest.sh"); // clean nginx
}

run();
