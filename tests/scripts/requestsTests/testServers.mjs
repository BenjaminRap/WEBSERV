import * as childProcess from "child_process"

const nginxUrl = "http://localhost:8181"
const werbservUrl = "http://localhost:8080"

const COLOR_RESET = "\x1b[0m"
const COLOR_RED = "\x1b[31m"
const COLOR_GREEN = "\x1b[32m"
const COLOR_BLUE = "\x1b[34m"
const COLOR_MAGENTA = "\x1b[35m"
const COLOR_CYAN = "\x1b[36m"

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
		console.log(prefix + COLOR_RED + "[KO] nginx|websev " + nginxValue + " | " + webservValue);
	else
		console.log(prefix + COLOR_GREEN + "[OK] " + nginxValue);
	console.log(COLOR_RESET);
}

function	getStatus(response)
{
	return (response.status + "|" + response.statusText);
}

function	isError(response)
{
	return (response.status >= 400);
}

export async function	compareRequests(target, method, body, headers)
{
	try
	{
		console.log("target : " + COLOR_BLUE + target + COLOR_RESET);
		const nginxResponse = await makeRequest(nginxUrl + target, method, body, headers);
		const webservResponse = await makeRequest(werbservUrl + target, method, body, headers);

		//status code + text
		verify("status : ", getStatus(nginxResponse) , getStatus(webservResponse) );

		//redirection
		const nginxRedirection = nginxResponse.headers.get("Location");
		const webservRedirection = webservResponse.headers.get("Location");
		if (nginxRedirection !== null && webservRedirection !== null)
			verify("redirection : ", nginxRedirection, webservRedirection);

		//body
		const nginxBody = await nginxResponse.text();
		const webservBody = await webservResponse.text();
		if (!isError(nginxResponse) && !isError(webservResponse) // We don't have the same error pages
			&& nginxBody !== "" && webservBody !== ""
			&& !webservBody.includes("<a href=\"../\">") // We don't have the same autoIndex
			&& !nginxBody.includes("<a href=\"../\">")) // We don't have the same autoIndex
		{
			verify("body : ", nginxBody, webservBody);
		}
		
		if ((nginxResponse.status == 201 && webservResponse.status == 201)
			|| (nginxResponse.status == 204 && webservResponse.status == 204))
		{
			if (nginxResponse.status == 201)
				console.log(COLOR_CYAN + "checking if the file has been created with the right body : " + COLOR_RESET)
				else
				console.log(COLOR_CYAN + "checking if the file has been deleted: " + COLOR_RESET)
			await compareRequests(target, "GET", null, headers);

		}
	}
	catch (error)
	{
		console.log(error);	
	}
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

export function	printHeader(header)
{
	console.log("");
	const band = "|" + "-".repeat(header.length + 6) + "|";
	console.log(COLOR_MAGENTA + band + COLOR_RESET);
	console.log(COLOR_BLUE + "|   " + header + "   |" + COLOR_RESET);
	console.log(COLOR_MAGENTA + band + COLOR_RESET);
}
