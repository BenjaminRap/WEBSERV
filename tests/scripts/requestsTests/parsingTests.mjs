import { compareBadRequests, compareBadRequestWithValues  } from "./compareRequests.mjs"
import { verifyServersAreRunning, exec, printHeader, COLOR_GREEN, COLOR_RED, COLOR_RESET } from "./utils.mjs"

const	printOK = false;

async function	runTest(header, message, target)
{
	printHeader(header);
	const	result = await compareBadRequests(message, target, printOK);
	if (result == true)
		console.log(COLOR_GREEN + "[OK] " + COLOR_RESET);
	else
		console.log(COLOR_RED + "[KO] " + COLOR_RESET);
}

async function	runTestWithStatusLine(header, statusLine, target)
{
	printHeader(header);
	const message = statusLine + "\r\nhost: dummy\r\nconnection: close\r\n\r\n";
	const	result = await  compareBadRequests(message, target, printOK);
	if (result == true)
		console.log(COLOR_GREEN + "[OK] " + COLOR_RESET);
	else
		console.log(COLOR_RED + "[KO] " + COLOR_RESET);
}

async function	runBadRequestTest(header, statusLine)
{
	printHeader(header + " -- Not nginx !");
	const message = statusLine + "\r\nhost: dummy\r\nconnection: close\r\n\r\n";
	const	result = await compareBadRequestWithValues(message, 400, "Bad Request", printOK);
	if (result == true)
		console.log(COLOR_GREEN + "[OK] " + COLOR_RESET);
	else
		console.log(COLOR_RED + "[KO] " + COLOR_RESET);
}

async function runTests()
{
	await runTestWithStatusLine("Unkown Method", "GLAGLA / HTTP/1.1", "/");
	await runTestWithStatusLine("Lower Case Method", "get /truc HTTP/1.1", "/truc");
	await runTestWithStatusLine("No Method", "/ HTTP/1.1", "/");
	await runTestWithStatusLine("Space Instead Of Method", "     / HTTP/1.1", "/");
	await runTestWithStatusLine("White Space Instead Of Method", "  \t\r \t  / HTTP/1.1", "/");
	await runTestWithStatusLine("Tab Instead Of Method", "\t / HTTP/1.1", "/");
	await runTestWithStatusLine("I Instead Of Method", "I / HTTP/1.1", "/");
	await runTestWithStatusLine("i Instead Of Method", "i / HTTP/1.1", "/");
	await runTestWithStatusLine("4 Instead Of Method", "4 / HTTP/1.1", "/");
	await runTestWithStatusLine("@ Instead Of Method", "@ / HTTP/1.1", "/");
	await runTestWithStatusLine("A@ Instead Of Method", "A@ / HTTP/1.1", "/");
	await runTestWithStatusLine("Tab Instead Of Space", "GET\t/\tHTTP/1.1", "/");
	await runTestWithStatusLine("No Spaces", "GET/HTTP/1.1", "/");
	await runTestWithStatusLine("Status Line Not Beginning With /", "GET truc HTTP/1.1", "truc");
	await runTestWithStatusLine("Invalid Version", "GET /truc HTTR/1.1", "/truc");
	await runTestWithStatusLine("Big Version Number", "GET /truc HTTP/5.1", "/truc");
	await runTestWithStatusLine("Lower Case Version", "GET /truc http/1.1", "/truc");
	await runTestWithStatusLine("Space Before Major", "GET /truc HTTP/ 1.1", "/truc");
	await runTestWithStatusLine("Space After Major", "GET /truc HTTP/1 .1", "/truc");
	await runTestWithStatusLine("Space Before Minor", "GET /truc HTTP/1. 1", "/truc");
	await runTestWithStatusLine("Finishing Whites Spaces", "GET /truc HTTP/1.1  \r \t ", "/truc");

	await runBadRequestTest("Overflowing Minor Number", "GET /truc HTTP/5.1200000000000000000000000000000");
	await runBadRequestTest("Overflowing Major Number", "GET /truc HTTP/2000000000000000000000000000005.1");
	await runBadRequestTest("Space After Minor", "GET /truc HTTP/1.1 ");
	await runBadRequestTest("Finishing Spaces", "GET /truc HTTP/1.1    ");
}

async function	run()
{
	verifyServersAreRunning();

	return (runTests());
}

run();
