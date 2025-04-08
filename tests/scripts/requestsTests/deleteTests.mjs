import { compareGoodRequests } from "./compareRequests.mjs"
import { verifyServersAreRunning, exec, printHeader } from "./utils.mjs"

function	runGoodDeleteTest(header, target)
{
	printHeader(header);
	return (compareGoodRequests(target, "DELETE", null, {}));
}

async function runTests()
{
	await runGoodDeleteTest("Normal Case", "/delete/full/classic");
	await runGoodDeleteTest("Normal Case (\"../\" in URL)","/delete/full/../../delete/full/noback");
	await runGoodDeleteTest("No Perm file","/delete/full/noperms");
	await runGoodDeleteTest("No Perm directory of file","/delete/cant/tryme");
	await runGoodDeleteTest("Only Read Perm directory of file","/delete/readme/deleteme");
	await runGoodDeleteTest("Not Found","/delete/emptwswdy");
	await runGoodDeleteTest("Url not end by \"/\"","/delete/folder/empty");
	await runGoodDeleteTest("Empty Directory","/delete/folder/empty/");
	await runGoodDeleteTest("Normal Case","/delete/folder/classic/");
	await runGoodDeleteTest("No Perms for parent dir","/delete/folder/nopermspa/");
	await runGoodDeleteTest("No Perms for the dir do del","/delete/folder/noperms/");
	await runGoodDeleteTest("Dir in Dir (But have no perms)","/delete/folder/dire/");
	await runGoodDeleteTest("Dir in Dir (Have no perms but empty)","/delete/folder/dire2/");
	await runGoodDeleteTest("Dir in Dir (Read Only but empty)","/delete/folder/dire3/");
	await runGoodDeleteTest("Normal Case ++++++","/delete/folder/dire4/");
}

async function	run()
{
	verifyServersAreRunning();

	exec("cd ../../webserv && ../scripts/requestsTests/initDeleteTest.sh"); // setup webserv
	exec("cd ../../nginx && ../scripts/requestsTests/initDeleteTest.sh"); // setup nginx

	await runTests();

	exec("cd ../../webserv && ../scripts/requestsTests/cleanDeleteTest.sh"); // clean webserv
	exec("cd ../../nginx && ../scripts/requestsTests/cleanDeleteTest.sh"); // clean nginx
}

run();
