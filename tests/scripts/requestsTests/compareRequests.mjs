import { exec, COLOR_RESET, COLOR_RED, COLOR_GREEN, COLOR_BLUE, COLOR_MAGENTA, COLOR_CYAN } from "./utils.mjs"
import { makeRequest } from "./makeRequest.mjs"
import { makeRawRequest } from "./makeRawRequest.mjs"

const protocol = "http"
const host = "0.0.0.0"
const nginxPort = 8181
const webservPort = 8080;

const nginxUrl = protocol + "://" + host + ":" + nginxPort;
const werbservUrl = protocol + "://" + host + ":" + webservPort;

function	verify(prefix, nginxValue, webservValue)
{
	if (nginxValue !== webservValue)
	{
		console.log(prefix + COLOR_RED + "[KO] nginx|webserv " + nginxValue + " | " + webservValue + COLOR_RESET);
		return (false);
	}
	console.log(prefix + COLOR_GREEN + "[OK] " + nginxValue + COLOR_RESET);
	return (true);
}

function	getStatus(response)
{
	return (response.status + "|" + response.statusText);
}

function	isError(response)
{
	return (response.status >= 400);
}

function	compareStatus(nginxResponse, webservResponse)
{
	//status code + text
	verify("status : ", getStatus(nginxResponse) , getStatus(webservResponse) );
}

function	compareRedirection(nginxResponse, webservResponse)
{
	//redirection
	const nginxRedirection = nginxResponse.headers.get("Location");
	const webservRedirection = webservResponse.headers.get("Location");
	if (nginxRedirection !== null && webservRedirection !== null)
			return (verify("redirection : ", nginxRedirection, webservRedirection));
	return (true);
}

async function	compareBody(nginxResponse, webservResponse)
{
	//body
	const nginxBody = await nginxResponse.text();
	const webservBody = await webservResponse.text();
	if (!isError(nginxResponse) && !isError(webservResponse) // We don't have the same error pages
		&& nginxBody !== "" && webservBody !== ""
		&& !webservBody.includes("<a href=\"../\">") // We don't have the same autoIndex
		&& !nginxBody.includes("<a href=\"../\">")) // We don't have the same autoIndex
	{
		return (verify("body : ", nginxBody, webservBody));
	}
	return (true);
}

function	compareRequestEffect(target, nginxResponse, webservResponse)
{
	if ((nginxResponse.status == 201 && webservResponse.status == 201)
		|| (nginxResponse.status == 204 && webservResponse.status == 204))
	{
		if (nginxResponse.status == 201)
			console.log(COLOR_CYAN + "checking if the file has been created with the right body : " + COLOR_RESET)
		else
			console.log(COLOR_CYAN + "checking if the file has been deleted: " + COLOR_RESET)
		return (compareGoodRequests(target, "GET", null, []));
	}
	return (true);
}

async function	compareRequests(target, nginxResponse, webservResponse)
{
	let	succeed = true;

	succeed = compareStatus(nginxResponse, webservResponse) && succeed;
	succeed = compareRedirection(nginxResponse, webservResponse) && succeed;
	succeed = await compareBody(nginxResponse, webservResponse) && succeed;
	succeed = compareRequestEffect(target, nginxResponse, webservResponse) && succeed;
	return (succeed);
}

export async function	compareGoodRequests(target, method, body, headers)
{
	console.log("target : " + COLOR_BLUE + target + COLOR_RESET);
	const nginxResponse = await makeRequest(nginxUrl + target, method, body, headers);
	const webservResponse = await makeRequest(werbservUrl + target, method, body, headers);
	return (compareRequests(target, nginxResponse, webservResponse));
}

export async function	compareBadRequests(message, target)
{
	console.log("message : " + message);
	const nginxResponse = await makeRawRequest(host, nginxPort, message);
	const webservResponse = await makeRawRequest(host, webservPort, message);
	return (compareRequests(target, nginxResponse, webservResponse));
}
