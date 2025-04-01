import { compareRequests, verifyServersAreRunning, exec, printHeader } from "./testServers.mjs"

async function	runDeleteTest(header, target)
{
	printHeader(test);
	await compareRequests(target, "DELETE", null, {});
}

async function runTests()
{
	await runDeleteTest("Normal Case", "/delete/full/classic");
	await runDeleteTest("Normal Case (\"../\" in URL)","/delete/full/../../delete/full/noback");
	await runDeleteTest("No Perm file","/delete/full/noperms");
	await runDeleteTest("No Perm directory of file","/delete/cant/tryme");
	await runDeleteTest("Only Read Perm directory of file","/delete/readme/deleteme");
	await runDeleteTest("Not Found","/delete/emptwswdy");
	await runDeleteTest("Url not end by \"/\"","/delete/folder/empty");
	await runDeleteTest("Empty Directory","/delete/folder/empty/");
	await runDeleteTest("Normal Case","/delete/folder/classic/");
	await runDeleteTest("No Perms for parent dir","/delete/folder/nopermspa/");
	await runDeleteTest("No Perms for the dir do del","/delete/folder/noperms/");
	await runDeleteTest("Dir in Dir (But have no perms)","/delete/folder/dire/");
	await runDeleteTest("Dir in Dir (Have no perms but empty)","/delete/folder/dire2/");
	await runDeleteTest("Dir in Dir (Read Only but empty)","/delete/folder/dire3/");
	await runDeleteTest("Normal Case ++++++","/delete/folder/dire4/");
}

async function	run()
{
	verifyServersAreRunning();

	exec("mkdir -p ../../../code/unitTest && cd ../../../code/unitTest && ../../tests/scripts/requestsTests/initDeleteTest.sh"); // setup webserv
	exec("cd ../../website && ../scripts/requestsTests/initDeleteTest.sh"); // setup nginx

	await runTests();

	exec("cd ../../../code/unitTest && ../../tests/scripts/requestsTests/cleanDeleteTest.sh"); // clean webserv
	exec("cd ../../website && ../scripts/requestsTests/cleanDeleteTest.sh"); // clean nginx
}

run();
