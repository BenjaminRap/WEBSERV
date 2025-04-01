import * as childProcess from "child_process"

const nginxUrl = "http://localhost:8181"
const werbservUrl = "http://localhost:8080"

const COLOR_RESET = "\x1b[0m"
const COLOR_RED = "\x1b[31m"
const COLOR_GREEN = "\x1b[32m"

async function	makeRequest(url, method, body, headers)
{
	const response = await fetch(url, {
		method: method,
		redirect: 'manual',
		body: body,
		headers: headers
	});
	return (response);
}

function	verify(prefix, nginxValue, webservValue)
{
	if (nginxValue !== webservValue)
		console.log(prefix, COLOR_RED, "[KO] : nginx : ", nginxValue, " /= webserv : ", webservValue);
	else
		console.log(prefix, COLOR_GREEN, "[OK] : ", nginxValue);
	console.log(COLOR_RESET);
}

export async function	compareRequests(target, method, body, headers)
{
	console.log(target);
	const nginxResponse = await makeRequest(nginxUrl + target, method, body, headers);
	const webservResponse = await makeRequest(werbservUrl + target, method, body, headers);
	verify("status : ", nginxResponse.status, webservResponse.status);
	verify("statusText : ", nginxResponse.statusText, webservResponse.statusText);
	const nginxRedirection = nginxResponse.headers.get("Location");
	const webservRedirection = webservResponse.headers.get("Location");
	if (nginxRedirection != null && webservRedirection != null)
		verify("redirection : ", nginxRedirection, webservRedirection);
	console.log("");
}

export function	exec(command)
{
	return (childProcess.spawnSync(command, { shell: true }));
}

export function	verifyServersAreRunning()
{
	{
		const result = exec("curl -I localhost:8181 > /dev/null 2>&1")

		if (result.status != 0)
		{
			console.log("Nginx server not running, exiting")
			process.exit(0);
		}
	}

	{
		const result = exec("curl -I localhost:8080 > /dev/null 2>&1");

		if (result.status != 0)
		{
			console.log("Nginx server not running, exiting")
			process.exit(0);
		}
	}
}
