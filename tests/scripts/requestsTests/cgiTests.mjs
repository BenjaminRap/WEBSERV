import { verifyServersAreRunning, exec, printHeader } from "./utils.mjs"
import { compareGoodRequestWithValues, compareBadRequestWithValues } from "./compareRequests.mjs";
import { COLOR_GREEN, COLOR_RED, COLOR_RESET, createChunkedRequest } from "./utils.mjs";

let	printOK = false;

async function	runGoodCgiTest(target, method, body, headers, statusCode, statusText)
{
	const result = await compareGoodRequestWithValues(target, method, body, headers, statusCode, statusText, printOK);

	if (result == true)
		console.log(COLOR_GREEN + "[OK] " + COLOR_RESET);
	else
		console.log(COLOR_RED + "[KO] " + COLOR_RESET);
	return (result);
}

async function	sendCgiChunkedRequest(target, headers, chunks, trailers)
{
	const	message = createChunkedRequest(target, headers, chunks, trailers);
	const	result = await compareBadRequestWithValues(message, 200, "OK", printOK);

	if (result == true)
		console.log(COLOR_GREEN + "[OK] " + COLOR_RESET);
	else
		console.log(COLOR_RED + "[KO] " + COLOR_RESET);
	return (result);
}

async function runTests()
{
	let	succeed = true;

	const	defaultHeaders = [ "Host: host:port", "Connection: close", "Transfer-Encoding: chunked" ];

	printHeader("Classic Case");
	succeed = await runGoodCgiTest("/cgi/cgiTestClassic.cgi", "GET", null, {}, 200, "OK") && succeed;

	printHeader("Chunked Body Output");
	succeed = await runGoodCgiTest("/cgi/cgiBodyChunked.cgi", "GET", null, {}, 200, "OK") && succeed;

	printHeader("Cgi Body Too Short");
	succeed = await runGoodCgiTest("/cgi/cgiBodyTooShort.cgi", "GET", null, {}, 200, "OK") && succeed;

	printHeader("Cgi Error In Headers");
	succeed = await runGoodCgiTest("/cgi/cgiErrorInHeaders.cgi", "GET", null, {}, 502, "Bad Gateway") && succeed;

	printHeader("Cgi No Content Length");
	succeed = await runGoodCgiTest("/cgi/cgiNoContentLength.cgi", "GET", null, {}, 200, "OK") && succeed;

	printHeader("Cgi No Content Type");
	succeed = await runGoodCgiTest("/cgi/cgiNoContentType.cgi", "GET", null, {}, 502, "Bad Gateway") && succeed;

	printHeader("Cgi No Response");
	succeed = await runGoodCgiTest("/cgi/cgiNoResponse.cgi", "GET", null, {}, 502, "Bad Gateway") && succeed;

	printHeader("Cgi No Status Header");
	succeed = await runGoodCgiTest("/cgi/cgiNoStatusHeader.cgi", "GET", null, {}, 200, "OK") && succeed;

	printHeader("Cgi No Status Header But Location");
	succeed = await runGoodCgiTest("/cgi/cgiNoStatusHeaderAndLocation.cgi", "GET", null, {}, 302, "Found") && succeed;

	printHeader("Cgi Printing Every Variables");
	succeed = await runGoodCgiTest("/cgi/cgiPrintingEveryVariables.cgi", "GET", null, {
		"accept" : "text/*",
		"accept-language" : "fr",
		"user-agent" : "jeSaisPasCeQueCEst",
		"cookIE" : "qwuehx",
		"REFERER" : "truc"
	}, 200, "OK") && succeed;

	printHeader("Cgi Closed While Not Reading All");
	succeed = await runGoodCgiTest("/cgi/put/cgiCloseWhileNotReadingAll.cgiput", "PUT", "un dummy body", {
		"content-length" : "13"
	}, 200, "OK") && succeed;

	printHeader("Cgi Wrong Content Length 1");
	succeed = await runGoodCgiTest("/cgi/cgiWrongContentLength1.cgi", "GET", null, {}, 502, "Bad Gateway") && succeed;

	printHeader("Cgi Wrong Content Length 2");
	succeed = await runGoodCgiTest("/cgi/cgiWrongContentLength2.cgi", "GET", null, {}, 502, "Bad Gateway") && succeed;

	printHeader("Cgi Wrong Content Length 3");
	succeed = await runGoodCgiTest("/cgi/cgiWrongContentLength3.cgi", "GET", null, {}, 502, "Bad Gateway") && succeed;

	printHeader("Cgi Wrong Status 1");
	succeed = await runGoodCgiTest("/cgi/cgiWrongStatus1.cgi", "GET", null, {}, 502, "Bad Gateway") && succeed;

	printHeader("Cgi Wrong Status 2");
	succeed = await runGoodCgiTest("/cgi/cgiWrongStatus2.cgi", "GET", null, {}, 502, "Bad Gateway") && succeed;

	printHeader("Cgi Wrong Status 3");
	succeed = await runGoodCgiTest("/cgi/cgiWrongStatus3.cgi", "GET", null, {}, 502, "Bad Gateway") && succeed;

	printHeader("Cgi Status Without Text");
	succeed = await runGoodCgiTest("/cgi/cgiStatusWithoutText.cgi", "GET", null, {}, 200, "OK") && succeed;

	printHeader("Cgi Check Body");
	succeed = await runGoodCgiTest("/cgi/put/cgiCheckBody.cgiput", "PUT", "je suis le body", {}, 200, "OK") && succeed;

	printHeader("Cgi Check Chunked Body");
	succeed = await sendCgiChunkedRequest("/cgi/put/cgiCheckBody.cgiput", defaultHeaders, [ "je ", "suis ", "le ", "body"], []) && succeed;

	printHeader("Cgi Check Chunked Body With Trailers");
	succeed = await sendCgiChunkedRequest("/cgi/put/cgiCheckBody.cgiput", defaultHeaders, [ "je ", "suis ", "le ", "body"], ["test: truc", "machin: bidule"]) && succeed;

	// printHeader("Cgi Python");
	// succeed = await runGoodCgiTest("/cgi/python/cgiPythonTest.py", "GET", null, {}, 200, "OK") && succeed;
	//
	// printHeader("Cgi Php");
	// succeed = await runGoodCgiTest("/cgi/php/cgiPhpTest.php", "GET", null, {},  200, "OK") && succeed;

	if (succeed)
		printHeader("Everything Done : " + COLOR_GREEN + "[OK] " + COLOR_RESET);
	else
		printHeader("Everything Done : " + COLOR_RED + "[KO] " + COLOR_RESET);
}

async function	run()
{
	verifyServersAreRunning();

	exec("cd ../../webserv && ../scripts/requestsTests/initCgiTest.sh"); // setup webserv

	await runTests();

	exec("cd ../../webserv && ../scripts/requestsTests/cleanCgiTest.sh"); // clean webserv
}

run();
