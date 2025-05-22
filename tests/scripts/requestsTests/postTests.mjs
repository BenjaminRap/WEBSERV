import { compareGoodRequests } from "./compareRequests.mjs"
import { verifyServersAreRunning, exec, printHeader, COLOR_GREEN, COLOR_RED, COLOR_RESET } from "./utils.mjs"

const	printOK = true;

async function	runGoodPostTest(target)
{
	const	result = await compareGoodRequests(target, "POST", null, {}, printOK);
	if (result == true)
		console.log(COLOR_GREEN + "[OK] " + COLOR_RESET);
	else
		console.log(COLOR_RED + "[KO] " + COLOR_RESET);
	return (result);
}

async function runTests()
{
	let	succeed = true;

	// printHeader("Classic Case");
	// succeed = await runGoodPostTest("/post/main.html") && succeed;
	// succeed = await runGoodPostTest("/post/fake/main.cpp") && succeed;
	// succeed = await runGoodPostTest("/post/fake/../main.html") && succeed;
	// succeed = await runGoodPostTest("/post/fake/../../../../../../../../main.html") && succeed;
	// succeed = await runGoodPostTest("/post/../main.html") && succeed;
	//
	// printHeader("Index Case");
	// succeed = await runGoodPostTest("/post/srcs/") && succeed;
	//
	printHeader("Redirection Case");
	succeed = await runGoodPostTest("/post/srcs") && succeed;
	//
	// printHeader("403 Case");
	// succeed = await runGoodPostTest("/post/fake/") && succeed;
	// succeed = await runGoodPostTest("/post/nonono/") && succeed;
	//
	// printHeader("405 Case");
	// succeed = await runGoodPostTest("/post/405/") && succeed;
	//
	// printHeader("404 Case");
	// succeed = await runGoodPostTest("/unitTest/uplo/") && succeed;
	// succeed = await runGoodPostTest("/gknrk") && succeed;
	// succeed = await runGoodPostTest("/bin/") && succeed;
	//
	// printHeader("Auto Index");
	// succeed = await runGoodPostTest("/post/auto/") && succeed;
	// succeed = await runGoodPostTest("/post/auto2/") && succeed;
	//
	// printHeader("Folder With File Name");
	// succeed = await runGoodPostTest("/post/truc.txt/") && succeed;

	printHeader("Index Specials");
	succeed = await runGoodPostTest("/post/indexIsFolder/") && succeed;
	succeed = await runGoodPostTest("/post/indexIsSymlink/") && succeed;
	succeed = await runGoodPostTest("/post/indexIsFolderAndFile/") && succeed;
	// succeed = await runGoodPostTest("/post/indexIsSymlinkAndFile/") && succeed;

	// printHeader("Fix Url Errors");
	// succeed = await runGoodPostTest("/post//main.html") && succeed;
	// succeed = await runGoodPostTest("/post/endWith./index.html") && succeed;

	if (succeed)
		printHeader("Everything Done : " + COLOR_GREEN + "[OK] " + COLOR_RESET);
	else
		printHeader("Everything Done : " + COLOR_RED + "[KO] " + COLOR_RESET);
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
