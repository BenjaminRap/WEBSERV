import { compareBadRequests } from "./compareRequests.mjs"
import { verifyServersAreRunning, exec, printHeader, generateString } from "./utils.mjs"

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
