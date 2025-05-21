import { verifyServersAreRunning, exec, printHeader } from "./utils.mjs"
import { compareGoodRequestWithValues } from "./compareRequests.mjs";
import { COLOR_GREEN, COLOR_RED, COLOR_RESET } from "./utils.mjs";

let	printOK = false;

async function	runGoodGgiTest(target, method, body, headers, statusCode, statusText)
{
	const result = await compareGoodRequestWithValues(target, method, body, headers, statusCode, statusText, printOK);

	if (result == true)
		console.log(COLOR_GREEN + "[OK] " + COLOR_RESET);
	else
		console.log(COLOR_RED + "[KO] " + COLOR_RESET);
}

async function runTests()
{
	printHeader("Classic Case");
	await runGoodGgiTest("/cgi/cgiTestClassic.cgi", "GET", null, {}, 200, "OK");
	printHeader("Chunked Body Output");
	await runGoodGgiTest("/cgi/cgiBodyChunked.cgi", "GET", null, {}, 200, "OK");
	printHeader("Cgi Body Too Short");
	await runGoodGgiTest("/cgi/cgiBodyTooShort.cgi", "GET", null, {}, 200, "OK");
	printHeader("Cgi Error In Headers");
	await runGoodGgiTest("/cgi/cgiErrorInHeaders.cgi", "GET", null, {}, 502, "Bad Gateway");
	printHeader("Cgi No Content Length");
	await runGoodGgiTest("/cgi/cgiNoContentLength.cgi", "GET", null, {}, 200, "OK");
	printHeader("Cgi No Content Type");
	await runGoodGgiTest("/cgi/cgiNoContentType.cgi", "GET", null, {}, 502, "Bad Gateway");
	printHeader("Cgi No Response");
	await runGoodGgiTest("/cgi/cgiNoResponse.cgi", "GET", null, {}, 502, "Bad Gateway");
	printHeader("Cgi No Status Header");
	await runGoodGgiTest("/cgi/cgiNoStatusHeader.cgi", "GET", null, {}, 200, "OK");
	printHeader("Cgi No Status Header But Location");
	await runGoodGgiTest("/cgi/cgiNoStatusHeaderAndLocation.cgi", "GET", null, {}, 302, "Found");
	printHeader("Cgi Printing Every Variables");
	await runGoodGgiTest("/cgi/cgiPrintingEveryVariables.cgi", "GET", null, {
		"accept" : "text/*",
		"accept-language" : "fr",
		"user-agent" : "jeSaisPasCeQueCEst",
		"cookIE" : "qwuehx",
		"REFERER" : "truc"
	}, 200, "OK");
	printHeader("Cgi Closed While Not Reading All");
	await runGoodGgiTest("/cgi/put/cgiCloseWhileNotReadingAll.cgiput", "PUT", "un dummy body", {
		"content-length" : "13"
	}, 200, "OK");
	printHeader("Cgi Wrong Content Length 1");
	await runGoodGgiTest("/cgi/cgiWrongContentLength1.cgi", "GET", null, {}, 502, "Bad Gateway");
	printHeader("Cgi Wrong Content Length 2");
	await runGoodGgiTest("/cgi/cgiWrongContentLength2.cgi", "GET", null, {}, 502, "Bad Gateway");
	printHeader("Cgi Wrong Content Length 3");
	await runGoodGgiTest("/cgi/cgiWrongContentLength3.cgi", "GET", null, {}, 502, "Bad Gateway");
	printHeader("Cgi Wrong Status 1");
	await runGoodGgiTest("/cgi/cgiWrongStatus1.cgi", "GET", null, {}, 502, "Bad Gateway");
	printHeader("Cgi Wrong Status 2");
	await runGoodGgiTest("/cgi/cgiWrongStatus2.cgi", "GET", null, {}, 502, "Bad Gateway");
	printHeader("Cgi Wrong Status 3");
	await runGoodGgiTest("/cgi/cgiWrongStatus3.cgi", "GET", null, {}, 502, "Bad Gateway");
	printHeader("Cgi Status Without Text");
	await runGoodGgiTest("/cgi/cgiStatusWithoutText.cgi", "GET", null, {}, 200, "OK");
}

async function	run()
{
	verifyServersAreRunning();

	exec("cd ../../webserv && ../scripts/requestsTests/initCgiTest.sh"); // setup webserv

	await runTests();

	exec("cd ../../webserv && ../scripts/requestsTests/cleanCgiTest.sh"); // clean webserv
}

run();
