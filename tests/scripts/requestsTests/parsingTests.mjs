import { compareBadRequests, compareBadRequestWithValues  } from "./compareRequests.mjs"
import { verifyServersAreRunning, exec, printHeader, COLOR_GREEN, COLOR_RED, COLOR_RESET } from "./utils.mjs"

const	printOK = false;

function	createRequest(statusLine, headers)
{
	let message = statusLine + "\r\n";
	headers.forEach((header) => {
		message += header + "\r\n";
	});
	message += "\r\n";
	return (message);
}

async function	runTestWithStatusLine(header, statusLine, target, headers)
{
	printHeader(header);

	const	message = createRequest(statusLine, headers);
	const	result = await  compareBadRequests(message, target, printOK);

	if (result == true)
		console.log(COLOR_GREEN + "[OK] " + COLOR_RESET);
	else
		console.log(COLOR_RED + "[KO] " + COLOR_RESET);
}

async function	runBadRequestTest(header, statusLine, headers)
{
	printHeader(header + " -- Not nginx !");

	const	message = createRequest(statusLine, headers);
	const	result = await compareBadRequestWithValues(message, 400, "Bad Request", printOK);

	if (result == true)
		console.log(COLOR_GREEN + "[OK] " + COLOR_RESET);
	else
		console.log(COLOR_RED + "[KO] " + COLOR_RESET);
}

async function runTests()
{
	const	defaultHeaders = [ "host: dummy", "connection: close" ];

	await runTestWithStatusLine("Unkown Method", "GLAGLA / HTTP/1.1", "/", defaultHeaders);
	await runTestWithStatusLine("Lower Case Method", "get /truc HTTP/1.1", "/truc", defaultHeaders);
	await runTestWithStatusLine("No Method", "/ HTTP/1.1", "/", defaultHeaders);
	await runTestWithStatusLine("Space Instead Of Method", "     / HTTP/1.1", "/", defaultHeaders);
	await runTestWithStatusLine("White Space Instead Of Method", "  \t\r \t  / HTTP/1.1", "/", defaultHeaders);
	await runTestWithStatusLine("Tab Instead Of Method", "\t / HTTP/1.1", "/", defaultHeaders);
	await runTestWithStatusLine("I Instead Of Method", "I / HTTP/1.1", "/", defaultHeaders);
	await runTestWithStatusLine("i Instead Of Method", "i / HTTP/1.1", "/", defaultHeaders);
	await runTestWithStatusLine("4 Instead Of Method", "4 / HTTP/1.1", "/", defaultHeaders);
	await runTestWithStatusLine("@ Instead Of Method", "@ / HTTP/1.1", "/", defaultHeaders);
	await runTestWithStatusLine("A@ Instead Of Method", "A@ / HTTP/1.1", "/", defaultHeaders);
	await runTestWithStatusLine("Tab Instead Of Space", "GET\t/\tHTTP/1.1", "/", defaultHeaders);
	await runTestWithStatusLine("No Spaces", "GET/HTTP/1.1", "/", defaultHeaders);
	await runTestWithStatusLine("Status Line Not Beginning With /", "GET truc HTTP/1.1", "truc", defaultHeaders);
	await runTestWithStatusLine("Invalid Version", "GET /truc HTTR/1.1", "/truc", defaultHeaders);
	await runTestWithStatusLine("Big Version Number", "GET /truc HTTP/5.1", "/truc", defaultHeaders);
	await runTestWithStatusLine("Lower Case Version", "GET /truc http/1.1", "/truc", defaultHeaders);
	await runTestWithStatusLine("Space Before Major", "GET /truc HTTP/ 1.1", "/truc", defaultHeaders);
	await runTestWithStatusLine("Space After Major", "GET /truc HTTP/1 .1", "/truc", defaultHeaders);
	await runTestWithStatusLine("Space Before Minor", "GET /truc HTTP/1. 1", "/truc", defaultHeaders);
	await runTestWithStatusLine("Finishing Whites Spaces", "GET /truc HTTP/1.1  \r \t ", "/truc", defaultHeaders);

	await runBadRequestTest("Overflowing Minor Number", "GET /truc HTTP/5.1200000000000000000000000000000", defaultHeaders);
	await runBadRequestTest("Overflowing Major Number", "GET /truc HTTP/2000000000000000000000000000005.1", defaultHeaders);
	await runBadRequestTest("Space After Minor", "GET /truc HTTP/1.1 ", defaultHeaders);
	await runBadRequestTest("Finishing Spaces", "GET /truc HTTP/1.1    ", defaultHeaders);
}

async function	run()
{
	verifyServersAreRunning();

	return (runTests());
}

run();
