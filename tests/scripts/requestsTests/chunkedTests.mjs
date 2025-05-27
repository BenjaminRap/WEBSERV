import { compareBadRequests, compareBadRequestWithValues } from "./compareRequests.mjs"
import { verifyServersAreRunning, exec, printHeader, generateString, randomInt, COLOR_GREEN, COLOR_RED, COLOR_RESET, createChunkedRequest } from "./utils.mjs"

let	failedTests = [];

function	randomStringArray(minArray, maxArray, minString, maxString)
{
    let		result = [];
	const	length = randomInt(minArray, maxArray);

    for (let i = 0; i < length; i++)
	{
        result.push(generateString(minString, maxString));
    }
    return (result);
}

async function	sendRawBadRequest(target, message)
{
	const	result = await compareBadRequestWithValues(message, 400, "Bad Request");

	if (result == true)
		console.log(COLOR_GREEN + "[OK] " + COLOR_RESET);
	else
	{
		console.log(COLOR_RED + "[KO] " + COLOR_RESET);
		failedTests.push(target);
	}
	return (result);
}

async function	sendGoodChunkedRequest(target, headers, chunks, trailers)
{
	const	message = createChunkedRequest(target, headers, chunks, trailers);
	const	result = await compareBadRequests(message, target);

	if (result == true)
		console.log(COLOR_GREEN + "[OK] " + COLOR_RESET);
	else
	{
		console.log(COLOR_RED + "[KO] " + COLOR_RESET);
		failedTests.push(target);
	}
	return (result);
}


async function	sendBadChunkedRequest(target, headers, chunks, trailers)
{
	const	message = createChunkedRequest(target, headers, chunks, trailers);
	const	result = await compareBadRequestWithValues(message, 400, "Bad Request");

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

	printHeader("Simple Chunked");
	await sendGoodChunkedRequest("/chunked/simple.txt", defaultHeaders, [ "je suis", "un test", "tu peux \n le voir" ], []);

	printHeader("Empty");
	await sendGoodChunkedRequest("/chunked/empty.txt", defaultHeaders, [], []);

	printHeader("One Chunk Empty");
	await sendGoodChunkedRequest("/chunked/oneEmpty.txt", defaultHeaders, ["testestets\r\r\n\r\n  ", "", "autre test"], []);

	printHeader("Random Small");
	await sendGoodChunkedRequest("/chunked/small.txt", defaultHeaders, randomStringArray(1, 5, 50, 100), []);

	printHeader("Random Medium");
	await sendGoodChunkedRequest("/chunked/medium.txt", defaultHeaders, randomStringArray(5, 20, 50, 100), []);

	printHeader("Random Big");
	await sendGoodChunkedRequest("/chunked/big.txt", defaultHeaders, randomStringArray(50, 100, 50, 100), []);

	printHeader("Random Huge");
	await sendGoodChunkedRequest("/chunked/huge.txt", defaultHeaders, randomStringArray(100000, 200000, 50, 100), []);

	printHeader("With Single Trailer");
	await sendGoodChunkedRequest("/chunked/singleTrailer.txt", defaultHeaders, randomStringArray(50, 100, 50, 100), ["Hello: mehe"]);

	printHeader("With Multiple Trailers");
	await sendGoodChunkedRequest("/chunked/multipleTrailers.txt", defaultHeaders, randomStringArray(50, 100, 50, 100), ["Hello: mehe", "Quit: now", "Cookies: nop"]);

	printHeader("With Invalid Trailer");
	await sendGoodChunkedRequest("/chunked/invalidTrailer.txt", defaultHeaders, randomStringArray(50, 100, 50, 100), [ "test=tru" ]);

	printHeader("No BreakLine On Size");
	await sendRawBadRequest("/chunked/noBreakLineOnSize",
"PUT /chunked/noBreakLineOnSize\r\n" +
"Host: nginx\r\n" +
"Connection: close\r\n" +
"Transfer-Encoding: chunked\r\n" +
"\r\n"  +
"5\r" + // error here
"je su\r\n" +
"0\r\n" +
"\r\n"
);

	printHeader("No Return Carriage On Size");
	await sendRawBadRequest("chunked/noReturnCarriageOnSize",
"PUT /chunked/noReturnCarriageOnSize\r\n" +
"Host: nginx\r\n" +
"Connection: close\r\n" +
"Transfer-Encoding: chunked\r\n" +
"\r\n"  +
"5\r\n" +
"je su\r\n" +
"0\n" + // error here
"\r\n"
);

	printHeader("No Return Carriage Nor BreakLine");
	await sendRawBadRequest("/chunked/noReturnCarriageNorBreakLineOnData",
"PUT /chunked/noReturnCarriageNorBreakLineOnSize\r\n" +
"Host: nginx\r\n" +
"Connection: close\r\n" +
"Transfer-Encoding: chunked\r\n" +
"\r\n"  +
"5\r\n" +
"je su\r\n" +
"A" + // error here
"je suis un\r\n" +
"0\r\n" +
"\r\n"
);

	printHeader("No BreakLine On Data");
	await sendRawBadRequest("chunked/noBreakLineOnData",
"PUT /chunked/noBreakLineOnData\r\n" +
"Host: nginx\r\n" +
"Connection: close\r\n" +
"Transfer-Encoding: chunked\r\n" +
"\r\n"  +
"5\r\n" +
"je su\r" + // error here
"0\r\n" +
"\r\n"
);

	printHeader("No Return Carriage On Data");
	await sendRawBadRequest("/chunked/noReturnCarriageOnData",
"PUT /chunked/noReturnCarriageOnData\r\n" +
"Host: nginx\r\n" +
"Connection: close\r\n" +
"Transfer-Encoding: chunked\r\n" +
"\r\n"  +
"5\r\n" +
"je su\n" + // error here
"0\n" +
"\r\n"
);

	printHeader("No Return Carriage Nor BreakLine");
	await sendRawBadRequest("/chunked/noReturnCarriageNorBreakLineOnData",
"PUT /chunked/noReturnCarriageNorBreakLineOnData\r\n" +
"Host: nginx\r\n" +
"Connection: close\r\n" +
"Transfer-Encoding: chunked\r\n" +
"\r\n"  +
"5\r\n" +
"je su\r\n" +
"A\r\b" +
"je suis un" + // error here
"0\r\n" +
"\r\n"
);


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

	exec("cd ../../webserv && ../scripts/requestsTests/initChunkedTest.sh"); // setup webserv
	exec("cd ../../nginx && ../scripts/requestsTests/initChunkedTest.sh"); // setup nginx

	await runTests();

	exec("cd ../../webserv && ../scripts/requestsTests/cleanChunkedTest.sh"); // clean webserv
	exec("cd ../../nginx && ../scripts/requestsTests/cleanChunkedTest.sh"); // clean nginx
}

run();
