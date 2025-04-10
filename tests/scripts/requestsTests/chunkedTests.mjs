import { compareBadRequests } from "./compareRequests.mjs"
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

async function	sendGoodChunkedRequest(header, target, headers, chunks, trailers)
{
	printHeader(header);
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
	const	result = await compareBadRequests(message, target, printOK);
	if (result == true)
		console.log(COLOR_GREEN + "[OK] " + COLOR_RESET);
	else
		console.log(COLOR_RED + "[KO] " + COLOR_RESET);
}

async function runTests()
{
	const	defaultHeaders = [ "Host: nginx", "Connection: close", "Transfer-Encoding: chunked" ];

	await sendGoodChunkedRequest("Simple Chunked", "/chunked/simple.txt", defaultHeaders, [ "je suis", "un test", "tu peux \n le voir" ], []);
	await sendGoodChunkedRequest("Empty", "/chunked/empty.txt", defaultHeaders, [], []);
	await sendGoodChunkedRequest("Random Small", "/chunked/small.txt", defaultHeaders, randomStringArray(1, 5, 50, 100), []);
	await sendGoodChunkedRequest("Random Medium", "/chunked/medium.txt", defaultHeaders, randomStringArray(5, 20, 50, 100), []);
	await sendGoodChunkedRequest("Random Big", "/chunked/big.txt", defaultHeaders, randomStringArray(50, 100, 50, 100), []);
	await sendGoodChunkedRequest("Random Huge", "/chunked/huge.txt", defaultHeaders, randomStringArray(1000, 2000, 50, 100), []);
	await sendGoodChunkedRequest("With Trailers", "/chunked/trailers.txt", defaultHeaders, randomStringArray(1000, 2000, 50, 100), ["Hello: mehe"]);
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
