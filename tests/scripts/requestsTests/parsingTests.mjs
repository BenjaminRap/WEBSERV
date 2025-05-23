import { compareBadRequests, compareBadRequestWithValues  } from "./compareRequests.mjs"
import { verifyServersAreRunning, exec, printHeader, COLOR_GREEN, COLOR_RED, COLOR_RESET } from "./utils.mjs"

function	createRequest(statusLine, headers)
{
	let message = statusLine + "\r\n";
	headers.forEach((header) => {
		message += header + "\r\n";
	});
	message += "\r\n";
	return (message);
}

async function	runTestWithStatusLine(statusLine, target, headers)
{
	const	message = createRequest(statusLine, headers);
	const	result = await  compareBadRequests(message, target);

	if (result == true)
		console.log(COLOR_GREEN + "[OK] " + COLOR_RESET);
	else
		console.log(COLOR_RED + "[KO] " + COLOR_RESET);
	return (result);
}

async function	runBadRequestTest(statusLine, headers)
{
	const	message = createRequest(statusLine, headers);
	const	result = await compareBadRequestWithValues(message, 400, "Bad Request");

	if (result == true)
		console.log(COLOR_GREEN + "[OK] " + COLOR_RESET);
	else
		console.log(COLOR_RED + "[KO] " + COLOR_RESET);
	return (result);
}

async function runTests()
{
	let	succeed = true;

	const	defaultHeaders = [ "host: dummy", "connection: close" ];

	printHeader("Unkown Method");
	succeed = await runTestWithStatusLine("GLAGLA / HTTP/1.1", "/", defaultHeaders) && succeed;

	printHeader("Lower Case Method");
	succeed = await runTestWithStatusLine("get /truc HTTP/1.1", "/truc", defaultHeaders) && succeed;

	printHeader("No Method");
	succeed = await runTestWithStatusLine("/ HTTP/1.1", "/", defaultHeaders) && succeed;

	printHeader("Space Instead Of Method");
	succeed = await runTestWithStatusLine("     / HTTP/1.1", "/", defaultHeaders) && succeed;

	printHeader("White Space Instead Of Method");
	succeed = await runTestWithStatusLine("  \t\r \t  / HTTP/1.1", "/", defaultHeaders) && succeed;

	printHeader("Tab Instead Of Method");
	succeed = await runTestWithStatusLine("\t / HTTP/1.1", "/", defaultHeaders) && succeed;

	printHeader("I Instead Of Method");
	succeed = await runTestWithStatusLine("I / HTTP/1.1", "/", defaultHeaders) && succeed;

	printHeader("i Instead Of Method");
	succeed = await runTestWithStatusLine("i / HTTP/1.1", "/", defaultHeaders) && succeed;

	printHeader("4 Instead Of Method");
	succeed = await runTestWithStatusLine("4 / HTTP/1.1", "/", defaultHeaders) && succeed;

	printHeader("@ Instead Of Method");
	succeed = await runTestWithStatusLine("@ / HTTP/1.1", "/", defaultHeaders) && succeed;

	printHeader("A@ Instead Of Method");
	succeed = await runTestWithStatusLine("A@ / HTTP/1.1", "/", defaultHeaders) && succeed;

	printHeader("Tab Instead Of Space");
	succeed = await runTestWithStatusLine("GET\t/\tHTTP/1.1", "/", defaultHeaders) && succeed;

	printHeader("No Spaces");
	succeed = await runTestWithStatusLine("GET/HTTP/1.1", "/", defaultHeaders) && succeed;

	printHeader("Status Line Not Beginning With /");
	succeed = await runTestWithStatusLine("GET truc HTTP/1.1", "truc", defaultHeaders) && succeed;

	printHeader("Invalid Version");
	succeed = await runTestWithStatusLine("GET /truc HTTR/1.1", "/truc", defaultHeaders) && succeed;

	printHeader("Big Version Number");
	succeed = await runTestWithStatusLine("GET /truc HTTP/5.1", "/truc", defaultHeaders) && succeed;

	printHeader("Lower Case Version");
	succeed = await runTestWithStatusLine("GET /truc http/1.1", "/truc", defaultHeaders) && succeed;

	printHeader("Space Before Major");
	succeed = await runTestWithStatusLine("GET /truc HTTP/ 1.1", "/truc", defaultHeaders) && succeed;

	printHeader("Space After Major");
	succeed = await runTestWithStatusLine("GET /truc HTTP/1 .1", "/truc", defaultHeaders) && succeed;

	printHeader("Space Before Minor");
	succeed = await runTestWithStatusLine("GET /truc HTTP/1. 1", "/truc", defaultHeaders) && succeed;

	printHeader("Finishing Whites Spaces");
	succeed = await runTestWithStatusLine("GET /truc HTTP/1.1  \r \t ", "/truc", defaultHeaders) && succeed;


	printHeader("Overflowing Minor Number");
	succeed = await runBadRequestTest("GET /truc HTTP/5.1200000000000000000000000000000", defaultHeaders) && succeed;

	printHeader("Overflowing Major Number");
	succeed = await runBadRequestTest("GET /truc HTTP/2000000000000000000000000000005.1", defaultHeaders) && succeed;

	printHeader("Space After Minor");
	succeed = await runBadRequestTest("GET /truc HTTP/1.1 ", defaultHeaders) && succeed;

	printHeader("Finishing Spaces");
	succeed = await runBadRequestTest("GET /truc HTTP/1.1    ", defaultHeaders) && succeed;

	if (succeed)
		printHeader("Everything Done : " + COLOR_GREEN + "[OK] " + COLOR_RESET);
	else
		printHeader("Everything Done : " + COLOR_RED + "[KO] " + COLOR_RESET);
}

async function	run()
{
	verifyServersAreRunning();

	return (runTests());
}

run();
