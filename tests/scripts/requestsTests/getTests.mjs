import { compareRequests, verifyServersAreRunning, exec, printHeader } from "./testServers.mjs"

async function	runGetTest(target)
{
	await compareRequests(target, "GET", null, {});
}

async function runTests()
{
	printHeader("Classic Case");
	await runGetTest("/get/main.html");
	await runGetTest("/get/fake/main.cpp");
	await runGetTest("/get/fake/../main.html");
	await runGetTest("/get/fake/../../../../../../../../main.html");
	await runGetTest("/get/../main.html");

	printHeader("Index Case");
	await runGetTest("/get/srcs/");

	printHeader("Redirection Case");
	await runGetTest("/get/srcs");

	printHeader("403 Case");
	await runGetTest("/get/fake/");
	await runGetTest("/get/nonono/");

	printHeader("405 Case");
	await runGetTest("/get/405/");

	printHeader("404 Case");
	await runGetTest("/unitTest/uplo/");
	await runGetTest("/gknrk");
	await runGetTest("/bin/");

	printHeader("Auto Index");
	await runGetTest("/get/auto/");
	await runGetTest("/get/auto2/");

	printHeader("Folder With File Name");
	await runGetTest("/get/truc.txt/");

	printHeader("Index Specials");
	await runGetTest("/get/indexIsFolder/");
	await runGetTest("/get/indexIsSymlink/");
	await runGetTest("/get/indexIsFolderAndFile/");
	await runGetTest("/get/indexIsSymlinkAndFile/");

	printHeader("Fix Url Errors");
	await runGetTest("/get//main.html");
	await runGetTest("/get/endWith./index.html");
}

async function	run()
{
	verifyServersAreRunning();

	exec("mkdir -p ../../../code/unitTest && cd ../../../code/unitTest && ../../tests/scripts/requestsTests/initGetTest.sh"); // setup webserv
	exec("cd ../../website && ../scripts/requestsTests/initGetTest.sh"); // setup nginx

	await runTests();

	exec("cd ../../../code/unitTest && ../../tests/scripts/requestsTests/cleanGetTest.sh"); // clean webserv
	exec("cd ../../website && ../scripts/requestsTests/cleanGetTest.sh"); // clean nginx
}

run();
