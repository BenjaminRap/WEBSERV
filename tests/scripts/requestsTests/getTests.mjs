import { compareRequests, verifyServersAreRunning, exec } from "./testServers.mjs"

async function	runGetTest(target)
{
	await compareRequests(target, "GET", null, {});
}

async function runTests()
{
	await runGetTest("/get/main.html");
	await runGetTest("/get/fake/main.cpp");
	await runGetTest("/get/fake/../main.html");
	await runGetTest("/get/fake/../../../../../../../../main.html");
	await runGetTest("/get/../main.html");
	await runGetTest("/get/srcs/");
	await runGetTest("/get/srcs");
	await runGetTest("/get/fake/");
	await runGetTest("/get/nonono/");
	await runGetTest("/get/405/");
	await runGetTest("/unitTest/uplo/");
	await runGetTest("/gknrk");
	await runGetTest("/bin/");
	await runGetTest("/get/auto/");
	await runGetTest("/get/auto2/");
	await runGetTest("/get/truc.txt/");
	await runGetTest("/get/indexIsFolder/");
	await runGetTest("/get/indexIsSymlink/");
	await runGetTest("/get/indexIsFolderAndFile/");
	await runGetTest("/get/indexIsSymlinkAndFile/");
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
