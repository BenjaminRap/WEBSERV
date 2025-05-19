import { makeRequest } from "./makeRequest.mjs"
import { verifyServersAreRunning, exec, printHeader } from "./utils.mjs"
import { webservUrl } from "./hosts.mjs"

async function	runGoodGgiTest(target, method, body, headers)
{
	const response = await makeRequest(webservUrl + target, method, body, headers);
	console.log(response.status);
	console.log(response.statusText);
	console.log(response.headers);
	console.log(await response.text());
}

async function runTests()
{
	printHeader("Classic Case");
	await runGoodGgiTest("/cgi/cgiTest.cgi", "GET", null, {});
}

async function	run()
{
	verifyServersAreRunning();

	exec("cd ../../webserv && ../scripts/requestsTests/initCgiTest.sh"); // setup webserv

	await runTests();

	exec("cd ../../webserv && ../scripts/requestsTests/cleanCgiTest.sh"); // clean webserv
}

run();
