import { compareGoodRequests, compareBadRequests } from "./compareRequests.mjs"
import { verifyServersAreRunning, exec, printHeader } from "./utils.mjs"

const characters ='ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789';
const charactersLength = characters.length;

function	randomInt(min, max)
{
	return (Math.floor(Math.random() * (max - min) + min));
}

function	generateString(min, max)
{
    let result = ' ';
	const length = randomInt(min, max);

    for (let i = 0; i < length; i++)
	{
		const randomIndex = randomInt(0, charactersLength);
        result += characters[randomIndex];
    }
    return result;
}

function	runGoodPutTest(header, target)
{
	printHeader(header);
	return (compareGoodRequests(target, "PUT", generateString(10, 100), {}));
}

function	sendChunkedRequest(header, target, chunks)
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
	// await runGoodPutTest("Allowed Case", "/put/allowed/main.html");
	// await runGoodPutTest("Method Not Allowed Case", "/put/notAllowed/main.html");
	// await runGoodPutTest("No Right on folder Case", "/put/forbidden/main.html");
	// await runGoodPutTest("File Already existing With right Case", "/put/mainRight.html");
	// await runGoodPutTest("File Already existing With no right Case", "/put/mainNoRight.html");
	// await runGoodPutTest("Creating directory Case", "/put/directory/");
	// await runGoodPutTest("Creating directory that already exists, with right and empty Case", "/put/alreadyExistingDirEmpty/");
	// await runGoodPutTest("Creating directory that already exists, with right and not empty Case", "/put/alreadyExistingDir/");
	// await runGoodPutTest("Creating directory that already exists, with no right and empty Case", "/put/alreadyExistingDirEmptyNoRight/");
	// await runGoodPutTest("Creating directory that already exists, with no right and not empty Case", "/put/alreadyExistingDirNoRight/");

	await sendChunkedRequest("Simple Chunked", "/upload", [ "je suis", "un test", "tu peux \n le voir" ]);
	await sendChunkedRequest("Simple Chunked", "/put/allowed/bidule.bid", [ "je suis", "un test", "tu peux \n le voir" ]);
}

async function	run()
{
	verifyServersAreRunning();

	exec("cd ../../webserv && ../scripts/requestsTests/initPutTest.sh"); // setup webserv
	exec("cd ../../nginx && ../scripts/requestsTests/initPutTest.sh"); // setup nginx

	await runTests();

	exec("cd ../../webserv && ../scripts/requestsTests/cleanPutTest.sh"); // clean webserv
	exec("cd ../../nginx && ../scripts/requestsTests/cleanPutTest.sh"); // clean nginx
}

run();
