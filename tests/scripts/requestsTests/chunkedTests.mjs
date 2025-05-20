import { compareBadRequests, compareBadRequestWithValues } from "./compareRequests.mjs"
import { verifyServersAreRunning, exec, printHeader, generateString, randomInt, COLOR_GREEN, COLOR_RED, COLOR_RESET } from "./utils.mjs"

const	printOK = false;

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

function	createChunkedRequest(target, headers, chunks, trailers)
{
	let message = "PUT " + target + " HTTP/1.1\r\n";
	headers.forEach((header) => {
		message += header + "\r\n";
	});
	message += "\r\n";
	chunks.forEach((chunk) => {
		message += chunk.length.toString(16) + "\r\n"
		message += chunk + "\r\n"
	});
	message += "0\r\n";
	trailers.forEach((trailer) => {
		message += trailer + "\r\n";
	});
	message += "\r\n";
	return (message);
}

async function	sendRawBadRequest(header, message)
{
	printHeader(header);
	const	result = await compareBadRequestWithValues(message, 400, "Bad Request", printOK);

	if (result == true)
		console.log(COLOR_GREEN + "[OK] " + COLOR_RESET);
	else
		console.log(COLOR_RED + "[KO] " + COLOR_RESET);
}

async function	sendGoodChunkedRequest(header, target, headers, chunks, trailers)
{
	printHeader(header);

	const	message = createChunkedRequest(target, headers, chunks, trailers);
	const	result = await compareBadRequests(message, target, printOK);

	if (result == true)
		console.log(COLOR_GREEN + "[OK] " + COLOR_RESET);
	else
		console.log(COLOR_RED + "[KO] " + COLOR_RESET);
}


async function	sendBadChunkedRequest(header, target, headers, chunks, trailers)
{
	printHeader(header);

	const	message = createChunkedRequest(target, headers, chunks, trailers);
	const	result = await compareBadRequestWithValues(message, 400, "Bad Request", printOK);

	if (result == true)
		console.log(COLOR_GREEN + "[OK] " + COLOR_RESET);
	else
		console.log(COLOR_RED + "[KO] " + COLOR_RESET);
}

async function runTests()
{
	const	defaultHeaders = [ "Host: host:port", "Connection: close", "Transfer-Encoding: chunked" ];

	await sendGoodChunkedRequest("Simple Chunked", "/chunked/simple.txt", defaultHeaders, [ "je suis", "un test", "tu peux \n le voir" ], []);
	await sendGoodChunkedRequest("Empty", "/chunked/empty.txt", defaultHeaders, [], []);
	await sendBadChunkedRequest("One Chunk Empty", "/chunked/oneEmpty.txt", defaultHeaders, ["testestets\r\r\n\r\n  ", "", "autre test"], []);
	await sendGoodChunkedRequest("Random Small", "/chunked/small.txt", defaultHeaders, randomStringArray(1, 5, 50, 100), []);
	await sendGoodChunkedRequest("Random Medium", "/chunked/medium.txt", defaultHeaders, randomStringArray(5, 20, 50, 100), []);
	await sendGoodChunkedRequest("Random Big", "/chunked/big.txt", defaultHeaders, randomStringArray(50, 100, 50, 100), []);
	await sendGoodChunkedRequest("Random Huge", "/chunked/huge.txt", defaultHeaders, randomStringArray(100000, 200000, 50, 100), []);
	await sendGoodChunkedRequest("With Single Trailer", "/chunked/singleTrailer.txt", defaultHeaders, randomStringArray(50, 100, 50, 100), ["Hello: mehe"]);
	await sendGoodChunkedRequest("With Multiple Trailers", "/chunked/multipleTrailers.txt", defaultHeaders, randomStringArray(50, 100, 50, 100), ["Hello: mehe", "Quit: now", "Cookies: nop"]);
	await sendGoodChunkedRequest("With Invalid Trailer", "/chunked/invalidTrailer.txt", defaultHeaders, randomStringArray(50, 100, 50, 100), [ "test=tru" ]);
	await sendRawBadRequest("No BreakLine On Size", 
"PUT /chunked/noBreakLineOnSize\r\n" +
"Host: nginx\r\n" +
"Connection: close\r\n" +
"Transfer-Encoding: chunked\r\n" +
"\r\n"  +
"5\r" + // error here
"je su\r\n" +
"0\r\n" +
"\r\n"
)
	await sendRawBadRequest("No Return Carriage On Size", 
"PUT /chunked/noReturnCarriageOnSize\r\n" +
"Host: nginx\r\n" +
"Connection: close\r\n" +
"Transfer-Encoding: chunked\r\n" +
"\r\n"  +
"5\r\n" +
"je su\r\n" +
"0\n" + // error here
"\r\n"
)

	await sendRawBadRequest("No Return Carriage Nor BreakLine",
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
)
	await sendRawBadRequest("No BreakLine On Data",
"PUT /chunked/noBreakLineOnData\r\n" +
"Host: nginx\r\n" +
"Connection: close\r\n" +
"Transfer-Encoding: chunked\r\n" +
"\r\n"  +
"5\r\n" +
"je su\r" + // error here
"0\r\n" +
"\r\n"
)
	await sendRawBadRequest("No Return Carriage On Data",
"PUT /chunked/noReturnCarriageOnData\r\n" +
"Host: nginx\r\n" +
"Connection: close\r\n" +
"Transfer-Encoding: chunked\r\n" +
"\r\n"  +
"5\r\n" +
"je su\n" + // error here
"0\n" +
"\r\n"
)

	await sendRawBadRequest("No Return Carriage Nor BreakLine",
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
)
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
