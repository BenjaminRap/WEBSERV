import { makeRequest } from "./makeRequest.mjs"
import { verifyServersAreRunning, exec, printHeader } from "./utils.mjs"
import { webservUrl } from "./hosts.mjs"

async function	runGoodGgiTest(target, method, body, headers)
{
	const response = await makeRequest(webservUrl + target, method, body, headers);
	console.log(response);
}

async function runTests()
{
	printHeader("Classic Case");
	await runGoodGgiTest("/cgi/cgiTest.bin", "GET", null, {});
}

async function	run()
{
	verifyServersAreRunning();

	exec("cd ../../webserv && ../scripts/requestsTests/initGgiTest.sh"); // setup webserv
	exec("cd ../../nginx && ../scripts/requestsTests/initGgiTest.sh"); // setup nginx

	await runTests();

	exec("cd ../../webserv && ../scripts/requestsTests/cleanGgiTest.sh"); // clean webserv
	exec("cd ../../nginx && ../scripts/requestsTests/cleanGgiTest.sh"); // clean nginx
}

run();
