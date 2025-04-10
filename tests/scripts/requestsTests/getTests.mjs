import { compareGoodRequests } from "./compareRequests.mjs"
import { verifyServersAreRunning, exec, printHeader, COLOR_GREEN, COLOR_RED, COLOR_RESET } from "./utils.mjs"

const	print = false;

async function	runGoodGetTest(target)
{
	const	result = await compareGoodRequests(target, "GET", null, {}, print);
	if (result == true)
		console.log(COLOR_GREEN + "[OK] " + COLOR_RESET);
	else
		console.log(COLOR_RED + "[KO] " + COLOR_RESET);
}

async function runTests()
{
	printHeader("Classic Case");
	await runGoodGetTest("/get/main.html");
	await runGoodGetTest("/get/fake/main.cpp");
	await runGoodGetTest("/get/fake/../main.html");
	await runGoodGetTest("/get/fake/../../../../../../../../main.html");
	await runGoodGetTest("/get/../main.html");

	printHeader("Index Case");
	await runGoodGetTest("/get/srcs/");

	printHeader("Redirection Case");
	await runGoodGetTest("/get/srcs");

	printHeader("403 Case");
	await runGoodGetTest("/get/fake/");
	await runGoodGetTest("/get/nonono/");

	printHeader("405 Case");
	await runGoodGetTest("/get/405/");

	printHeader("404 Case");
	await runGoodGetTest("/unitTest/uplo/");
	await runGoodGetTest("/gknrk");
	await runGoodGetTest("/bin/");

	printHeader("Auto Index");
	await runGoodGetTest("/get/auto/");
	await runGoodGetTest("/get/auto2/");

	printHeader("Folder With File Name");
	await runGoodGetTest("/get/truc.txt/");

	printHeader("Index Specials");
	await runGoodGetTest("/get/indexIsFolder/");
	await runGoodGetTest("/get/indexIsSymlink/");
	await runGoodGetTest("/get/indexIsFolderAndFile/");
	await runGoodGetTest("/get/indexIsSymlinkAndFile/");

	printHeader("Fix Url Errors");
	await runGoodGetTest("/get//main.html");
	await runGoodGetTest("/get/endWith./index.html");
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
