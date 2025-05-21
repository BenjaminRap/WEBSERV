import { compareGoodRequests } from "./compareRequests.mjs"
import { verifyServersAreRunning, exec, printHeader, COLOR_GREEN, COLOR_RED, COLOR_RESET } from "./utils.mjs"

const	printOK = false;

async function	runGoodDeleteTest(target)
{
	const result = await compareGoodRequests(target, "DELETE", null, {}, printOK);
	if (result == true)
		console.log(COLOR_GREEN + "[OK] " + COLOR_RESET);
	else
		console.log(COLOR_RED + "[KO] " + COLOR_RESET);
	return (result);
}

async function runTests()
{
	let	succeed = true;

	printHeader("Normal Case");
	succeed = await runGoodDeleteTest("/delete/full/classic") && succeed;

	printHeader("Normal Case (\"../\" in URL)");
	succeed = await runGoodDeleteTest("/delete/full/../../delete/full/noback") && succeed;

	printHeader("No Perm file");
	succeed = await runGoodDeleteTest("/delete/full/noperms") && succeed;

	printHeader("No Perm directory of file");
	succeed = await runGoodDeleteTest("/delete/cant/tryme") && succeed;

	printHeader("Only Read Perm directory of file");
	succeed = await runGoodDeleteTest("/delete/readme/deleteme") && succeed;

	printHeader("Not Found");
	succeed = await runGoodDeleteTest("/delete/emptwswdy") && succeed;

	printHeader("Url not end by \"/\"");
	succeed = await runGoodDeleteTest("/delete/folder/empty") && succeed;

	printHeader("Empty Directory");
	succeed = await runGoodDeleteTest("/delete/folder/empty/") && succeed;

	printHeader("Normal Case");
	succeed = await runGoodDeleteTest("/delete/folder/classic/") && succeed;

	printHeader("No Perms for parent dir");
	succeed = await runGoodDeleteTest("/delete/folder/nopermspa/") && succeed;

	printHeader("No Perms for the dir do del");
	succeed = await runGoodDeleteTest("/delete/folder/noperms/") && succeed;

	printHeader("Dir in Dir (But have no perms)");
	succeed = await runGoodDeleteTest("/delete/folder/dire/") && succeed;

	printHeader("Dir in Dir (Have no perms but empty)");
	succeed = await runGoodDeleteTest("/delete/folder/dire2/") && succeed;

	printHeader("Dir in Dir (Read Only but empty)");
	succeed = await runGoodDeleteTest("/delete/folder/dire3/") && succeed;

	printHeader("Normal Case ++++++");
	succeed = await runGoodDeleteTest("/delete/folder/dire4/") && succeed;


	if (succeed)
		printHeader("Everything Done : " + COLOR_GREEN + "[OK] " + COLOR_RESET);
	else
		printHeader("Everything Done : " + COLOR_RED + "[KO] " + COLOR_RESET);
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
