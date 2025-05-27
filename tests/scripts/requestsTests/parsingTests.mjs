import { compareBadRequests, compareBadRequestWithValues  } from "./compareRequests.mjs"
import { verifyServersAreRunning, exec, printHeader, COLOR_GREEN, COLOR_RED, COLOR_RESET } from "./utils.mjs"

let	failedTests = [];

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
	{
		console.log(COLOR_RED + "[KO] " + COLOR_RESET);
		failedTests.push(target);
	}
	return (result);
}

async function	runBadRequestTest(statusLine, headers)
{
	const	message = createRequest(statusLine, headers);
	const	result = await compareBadRequestWithValues(message, 400, "Bad Request");

	if (result == true)
		console.log(COLOR_GREEN + "[OK] " + COLOR_RESET);
	else
	{
		console.log(COLOR_RED + "[KO] " + COLOR_RESET);
		failedTests.push(statusLine);
	}
	return (result);
}

async function runTests()
{
	const	defaultHeaders = [ "host: dummy", "connection: close" ];

	printHeader("Unkown Method");
	await runTestWithStatusLine("GLAGLA / HTTP/1.1", "/", defaultHeaders);

	printHeader("Lower Case Method");
	await runTestWithStatusLine("get /truc HTTP/1.1", "/truc", defaultHeaders);

	printHeader("No Method");
	await runTestWithStatusLine("/ HTTP/1.1", "/", defaultHeaders);

	printHeader("Space Instead Of Method");
	await runTestWithStatusLine("     / HTTP/1.1", "/", defaultHeaders);

	printHeader("White Space Instead Of Method");
	await runTestWithStatusLine("  \t\r \t  / HTTP/1.1", "/", defaultHeaders);

	printHeader("Tab Instead Of Method");
	await runTestWithStatusLine("\t / HTTP/1.1", "/", defaultHeaders);

	printHeader("I Instead Of Method");
	await runTestWithStatusLine("I / HTTP/1.1", "/", defaultHeaders);

	printHeader("i Instead Of Method");
	await runTestWithStatusLine("i / HTTP/1.1", "/", defaultHeaders);

	printHeader("4 Instead Of Method");
	await runTestWithStatusLine("4 / HTTP/1.1", "/", defaultHeaders);

	printHeader("@ Instead Of Method");
	await runTestWithStatusLine("@ / HTTP/1.1", "/", defaultHeaders);

	printHeader("A@ Instead Of Method");
	await runTestWithStatusLine("A@ / HTTP/1.1", "/", defaultHeaders);

	printHeader("Tab Instead Of Space");
	await runTestWithStatusLine("GET\t/\tHTTP/1.1", "/", defaultHeaders);

	printHeader("No Spaces");
	await runTestWithStatusLine("GET/HTTP/1.1", "/", defaultHeaders);

	printHeader("Status Line Not Beginning With /");
	await runTestWithStatusLine("GET truc HTTP/1.1", "truc", defaultHeaders);

	printHeader("Invalid Version");
	await runTestWithStatusLine("GET /truc HTTR/1.1", "/truc", defaultHeaders);

	printHeader("Big Version Number");
	await runTestWithStatusLine("GET /truc HTTP/5.1", "/truc", defaultHeaders);

	printHeader("Lower Case Version");
	await runTestWithStatusLine("GET /truc http/1.1", "/truc", defaultHeaders);

	printHeader("Space Before Major");
	await runTestWithStatusLine("GET /truc HTTP/ 1.1", "/truc", defaultHeaders);

	printHeader("Space After Major");
	await runTestWithStatusLine("GET /truc HTTP/1 .1", "/truc", defaultHeaders);

	printHeader("Space Before Minor");
	await runTestWithStatusLine("GET /truc HTTP/1. 1", "/truc", defaultHeaders);

	printHeader("Finishing Whites Spaces");
	await runTestWithStatusLine("GET /truc HTTP/1.1  \r \t ", "/truc", defaultHeaders);


	printHeader("Overflowing Minor Number");
	await runBadRequestTest("GET /truc HTTP/5.1200000000000000000000000000000", defaultHeaders);

	printHeader("Overflowing Major Number");
	await runBadRequestTest("GET /truc HTTP/2000000000000000000000000000005.1", defaultHeaders);

	printHeader("Space After Minor");
	await runBadRequestTest("GET /truc HTTP/1.1 ", defaultHeaders);

	printHeader("Finishing Spaces");
	await runBadRequestTest("GET /truc HTTP/1.1    ", defaultHeaders);

	if (failedTests.length == 0)
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
