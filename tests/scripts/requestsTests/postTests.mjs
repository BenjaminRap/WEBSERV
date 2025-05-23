import { compareGoodRequests } from "./compareRequests.mjs"
import { verifyServersAreRunning, exec, printHeader, COLOR_GREEN, COLOR_RED, COLOR_RESET } from "./utils.mjs"

let	failedTests = [];

async function	runGoodPostTest(target)
{
	const	result = await compareGoodRequests(target, "POST", null, {});
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
	printHeader("Classic Case");
	await runGoodPostTest("/post/main.html");
	await runGoodPostTest("/post/fake/main.cpp");
	await runGoodPostTest("/post/fake/../main.html");
	await runGoodPostTest("/post/fake/../../../../../../../../main.html");
	await runGoodPostTest("/post/../main.html");

	printHeader("Index Case");
	await runGoodPostTest("/post/srcs/");

	printHeader("Redirection Case");
	await runGoodPostTest("/post/srcs");

	printHeader("403 Case");
	await runGoodPostTest("/post/fake/");
	await runGoodPostTest("/post/nonono/");

	printHeader("405 Case");
	await runGoodPostTest("/post/405/");

	printHeader("404 Case");
	await runGoodPostTest("/unitTest/uplo/");
	await runGoodPostTest("/gknrk");
	await runGoodPostTest("/bin/");

	printHeader("Auto Index");
	await runGoodPostTest("/post/auto/");
	await runGoodPostTest("/post/auto2/");

	printHeader("Folder With File Name");
	await runGoodPostTest("/post/truc.txt/");

	printHeader("Index Specials");
	await runGoodPostTest("/post/indexIsFolder/");
	await runGoodPostTest("/post/indexIsSymlink/");
	await runGoodPostTest("/post/indexIsFolderAndFile/");
	await runGoodPostTest("/post/indexIsSymlinkAndFile/");

	printHeader("Fix Url Errors");
	await runGoodPostTest("/post//main.html");
	await runGoodPostTest("/post/endWith./index.html");

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

	exec("cd ../../webserv && ../scripts/requestsTests/initPostTest.sh"); // setup webserv
	exec("cd ../../nginx && ../scripts/requestsTests/initPostTest.sh"); // setup nginx

	await runTests();

	exec("cd ../../webserv && ../scripts/requestsTests/cleanPostTest.sh"); // clean webserv
	exec("cd ../../nginx && ../scripts/requestsTests/cleanPostTest.sh"); // clean nginx
}

run();
