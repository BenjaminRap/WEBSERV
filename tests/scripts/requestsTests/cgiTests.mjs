import { verifyServersAreRunning, exec, printHeader } from "./utils.mjs"
import { compareGoodRequestWithValues, compareBadRequestWithValues } from "./compareRequests.mjs";
import { COLOR_GREEN, COLOR_RED, COLOR_RESET, createChunkedRequest } from "./utils.mjs";

let	failedTests = [];

async function	runGoodCgiTest(target, method, body, headers, statusCode, statusText)
{
	const result = await compareGoodRequestWithValues(target, method, body, headers, statusCode, statusText);

	if (result == true)
		console.log(COLOR_GREEN + "[OK] " + COLOR_RESET);
	else
	{
		console.log(COLOR_RED + "[KO] " + COLOR_RESET);
		failedTests.push(target);
	}
	return (result);
}

async function	sendCgiChunkedRequest(target, headers, chunks, trailers)
{
	const	message = createChunkedRequest(target, headers, chunks, trailers);
	const	result = await compareBadRequestWithValues(message, 200, "OK");

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
	const	defaultHeaders = [ "Host: host:port", "Connection: close", "Transfer-Encoding: chunked" ];

	printHeader("Classic Case");
	await runGoodCgiTest("/cgi/cgiTestClassic.cgi", "GET", null, {}, 200, "OK");

	printHeader("Chunked Body Output");
	await runGoodCgiTest("/cgi/cgiBodyChunked.cgi", "GET", null, {}, 200, "OK");

	printHeader("Cgi Body Too Short");
	await runGoodCgiTest("/cgi/cgiBodyTooShort.cgi", "GET", null, {}, 200, "OK");

	printHeader("Cgi Error In Headers");
	await runGoodCgiTest("/cgi/cgiErrorInHeaders.cgi", "GET", null, {}, 502, "Bad Gateway");

	printHeader("Cgi No Content Length");
	await runGoodCgiTest("/cgi/cgiNoContentLength.cgi", "GET", null, {}, 200, "OK");

	printHeader("Cgi No Content Type");
	await runGoodCgiTest("/cgi/cgiNoContentType.cgi", "GET", null, {}, 502, "Bad Gateway");

	printHeader("Cgi No Response");
	await runGoodCgiTest("/cgi/cgiNoResponse.cgi", "GET", null, {}, 502, "Bad Gateway");

	printHeader("Cgi No Status Header");
	await runGoodCgiTest("/cgi/cgiNoStatusHeader.cgi", "GET", null, {}, 200, "OK");

	printHeader("Cgi No Status Header But Location");
	await runGoodCgiTest("/cgi/cgiNoStatusHeaderAndLocation.cgi", "GET", null, {}, 302, "Found");

	printHeader("Cgi Printing Every Variables");
	await runGoodCgiTest("/cgi/cgiPrintingEveryVariables.cgi", "GET", null, {
		"accept" : "text/*",
		"accept-language" : "fr",
		"user-agent" : "jeSaisPasCeQueCEst",
		"cookIE" : "qwuehx",
		"REFERER" : "truc"
	}, 200, "OK");

	printHeader("Cgi Closed While Not Reading All");
	await runGoodCgiTest("/cgi/put/cgiCloseWhileNotReadingAll.cgiput", "PUT", "un dummy body", {
		"content-length" : "13"
	}, 200, "OK");

	printHeader("Cgi Wrong Content Length 1");
	await runGoodCgiTest("/cgi/cgiWrongContentLength1.cgi", "GET", null, {}, 502, "Bad Gateway");

	printHeader("Cgi Wrong Content Length 2");
	await runGoodCgiTest("/cgi/cgiWrongContentLength2.cgi", "GET", null, {}, 502, "Bad Gateway");

	printHeader("Cgi Wrong Content Length 3");
	await runGoodCgiTest("/cgi/cgiWrongContentLength3.cgi", "GET", null, {}, 502, "Bad Gateway");

	printHeader("Cgi Wrong Status 1");
	await runGoodCgiTest("/cgi/cgiWrongStatus1.cgi", "GET", null, {}, 502, "Bad Gateway");

	printHeader("Cgi Wrong Status 2");
	await runGoodCgiTest("/cgi/cgiWrongStatus2.cgi", "GET", null, {}, 502, "Bad Gateway");

	printHeader("Cgi Wrong Status 3");
	await runGoodCgiTest("/cgi/cgiWrongStatus3.cgi", "GET", null, {}, 502, "Bad Gateway");

	printHeader("Cgi Status Without Text");
	await runGoodCgiTest("/cgi/cgiStatusWithoutText.cgi", "GET", null, {}, 200, "OK");

	printHeader("Cgi Check Body");
	await runGoodCgiTest("/cgi/put/cgiCheckBody.cgiput", "PUT", "je suis le body", {}, 200, "OK");

	printHeader("Cgi Check Chunked Body");
	await sendCgiChunkedRequest("/cgi/put/cgiCheckBody.cgiput", defaultHeaders, [ "je ", "suis ", "le ", "body"], []);

	printHeader("Cgi Check Chunked Body With Trailers");
	await sendCgiChunkedRequest("/cgi/put/cgiCheckBody.cgiput", defaultHeaders, [ "je ", "suis ", "le ", "body"], ["test: truc", "machin: bidule"]);

	printHeader("Cgi Test Sleep");
	await runGoodCgiTest("/cgi/cgiTestSleep.cgi", "GET", null, {}, 200, "OK");

	printHeader("Cgi Python");
	await runGoodCgiTest("/cgi/python/cgiPythonTest.py", "GET", null, {}, 200, "OK");

	printHeader("Cgi Php");
	await runGoodCgiTest("/cgi/php/cgiPhpTest.php", "GET", null, {},  200, "OK");

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

	exec("cd ../../webserv && ../scripts/requestsTests/initCgiTest.sh"); // setup webserv

	await runTests();

	exec("cd ../../webserv && ../scripts/requestsTests/cleanCgiTest.sh"); // clean webserv
}

run();
