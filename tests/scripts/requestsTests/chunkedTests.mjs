import { compareBadRequests } from "./compareRequests.mjs"
import { verifyServersAreRunning, exec, printHeader, generateString, randomInt } from "./utils.mjs"

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

function	sendGoodChunkedRequest(header, target, chunks)
{
	printHeader(header);
	let message = "PUT " + target + " HTTP/1.1\r\n";
	message += "Host: nginx\r\n";
	message += "Connection: close\r\n";
	message += "Transfer-Encoding: chunked\r\n"
	message += "\r\n";
	chunks.forEach((chunk) => {
		message += chunk.length.toString(16) + "\r\n"
		message += chunk + "\r\n"
	});
	message += "0\r\n\r\n";
	return (compareBadRequests(message, target));
}

async function runTests()
{
	await sendGoodChunkedRequest("Simple Chunked", "/chunked/simple.txt", [ "je suis", "un test", "tu peux \n le voir" ]);
	await sendGoodChunkedRequest("Empty", "/chunked/empty.txt", []);
	await sendGoodChunkedRequest("Random Small", "/chunked/small.txt", randomStringArray(1, 5, 50, 100));
	await sendGoodChunkedRequest("Random Medium", "/chunked/medium.txt", randomStringArray(5, 20, 50, 100));
	await sendGoodChunkedRequest("Random Big", "/chunked/big.txt", randomStringArray(50, 100, 50, 100));
	await sendGoodChunkedRequest("Random Huge", "/chunked/huge.txt", randomStringArray(1000, 2000, 50, 100));
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
