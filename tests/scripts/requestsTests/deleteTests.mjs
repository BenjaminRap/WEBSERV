import { compareGoodRequests } from "./compareRequests.mjs"
import { verifyServersAreRunning, exec, printHeader, COLOR_GREEN, COLOR_RED, COLOR_RESET } from "./utils.mjs"

let	failedTests = [];

async function	runGoodDeleteTest(target)
{
	const result = await compareGoodRequests(target, "DELETE", null, {});
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
	printHeader("Normal Case");
	await runGoodDeleteTest("/delete/full/classic");

	printHeader("Normal Case (\"../\" in URL)");
	await runGoodDeleteTest("/delete/full/../../delete/full/noback");

	printHeader("No Perm file");
	await runGoodDeleteTest("/delete/full/noperms");

	printHeader("No Perm directory of file");
	await runGoodDeleteTest("/delete/cant/tryme");

	printHeader("Only Read Perm directory of file");
	await runGoodDeleteTest("/delete/readme/deleteme");

	printHeader("Not Found");
	await runGoodDeleteTest("/delete/emptwswdy");

	printHeader("Url not end by \"/\"");
	await runGoodDeleteTest("/delete/folder/empty");

	printHeader("Empty Directory");
	await runGoodDeleteTest("/delete/folder/empty/");

	printHeader("Normal Case");
	await runGoodDeleteTest("/delete/folder/classic/");

	printHeader("No Perms for parent dir");
	await runGoodDeleteTest("/delete/folder/nopermspa/");

	printHeader("No Perms for the dir do del");
	await runGoodDeleteTest("/delete/folder/noperms/");

	printHeader("Dir in Dir (But have no perms)");
	await runGoodDeleteTest("/delete/folder/dire/");

	printHeader("Dir in Dir (Have no perms but empty)");
	await runGoodDeleteTest("/delete/folder/dire2/");

	printHeader("Dir in Dir (Read Only but empty)");
	await runGoodDeleteTest("/delete/folder/dire3/");

	printHeader("Normal Case ++++++");
	await runGoodDeleteTest("/delete/folder/dire4/");


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

	exec("cd ../../webserv && ../scripts/requestsTests/initDeleteTest.sh"); // setup webserv
	exec("cd ../../nginx && ../scripts/requestsTests/initDeleteTest.sh"); // setup nginx

	await runTests();

	exec("cd ../../webserv && ../scripts/requestsTests/cleanDeleteTest.sh"); // clean webserv
	exec("cd ../../nginx && ../scripts/requestsTests/cleanDeleteTest.sh"); // clean nginx
}

run();
