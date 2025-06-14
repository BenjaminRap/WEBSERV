import { compareGoodRequests } from "./compareRequests.mjs"
import { verifyServersAreRunning, exec, printHeader, COLOR_GREEN, COLOR_RED, COLOR_RESET } from "./utils.mjs"

let	failedTests = [];

async function	runGoodGetTest(target, expectedHeaders)
{
	const	result = await compareGoodRequests(target, "GET", null, {}, expectedHeaders);
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

	printHeader("Add_header Tests Not Always With Error")
	await runGoodGetTest("/add_header/get/srcs/");

	printHeader("Add_header Tests Always With Error")
	await runGoodGetTest("/add_header/always/get/srcs/", {"pragma":"truc"});

	printHeader("Add_header Tests Not Always Without Error")
	await runGoodGetTest("/add_header/get/srcs/main.cpp", {"accept":"text/html"});

	printHeader("Add_header Tests Always Without Error")
	await runGoodGetTest("/add_header/always/get/srcs/main.cpp", {"pragma":"truc"});

	printHeader("Same but with query string");
	await runGoodGetTest("/get/main.html?truc=var");
	await runGoodGetTest("/get/fake/main.cpp?truc=var");
	await runGoodGetTest("/get/fake/../main.html?truc=var");
	await runGoodGetTest("/get/fake/../../../../../../../../main.html?truc=var");
	await runGoodGetTest("/get/../main.html?truc=var");
	await runGoodGetTest("/get/srcs/?truc=var");
	await runGoodGetTest("/get/srcs?truc=var");
	await runGoodGetTest("/get/fake/?truc=var");
	await runGoodGetTest("/get/nonono/?truc=var");
	await runGoodGetTest("/get/405/?truc=var");
	await runGoodGetTest("/unitTest/uplo/?truc=var");
	await runGoodGetTest("/gknrk?truc=var");
	await runGoodGetTest("/get/auto2/?truc=var");
	await runGoodGetTest("/get/truc.txt/?truc=var");
	await runGoodGetTest("/get/indexIsFolder/?truc=var");
	await runGoodGetTest("/get/indexIsSymlink/?truc=var");
	await runGoodGetTest("/get/indexIsFolderAndFile/?truc=var");
	await runGoodGetTest("/get/indexIsSymlinkAndFile/?truc=var");
	await runGoodGetTest("/get//main.html?truc=var");
	await runGoodGetTest("/get/endWith./index.html?truc=var");

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

	exec("cd ../../webserv && ../scripts/requestsTests/initGetTest.sh"); // setup webserv
	exec("cp -pr ../../webserv/* ../../nginx/"); // setup nginx

	await runTests();

	exec("cd ../../webserv && ../scripts/requestsTests/cleanGetTest.sh"); // clean webserv
	exec("rm -rf ../../nginx/*"); // clean nginx
}

run();
