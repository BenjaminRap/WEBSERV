import { exec, COLOR_RESET, COLOR_RED, COLOR_GREEN, COLOR_BLUE, COLOR_MAGENTA, COLOR_CYAN } from "./utils.mjs"
import { makeRequest } from "./makeRequest.mjs"
import { makeRawRequest } from "./makeRawRequest.mjs"
import { nginxHost, webservHost, nginxPort, webservPort, nginxUrl, webservUrl } from "./hosts.mjs"

function	verify(prefix, nginxValue, webservValue, printOK)
{
	if (nginxValue !== webservValue)
	{
		console.log(prefix + COLOR_RED + "[KO] nginx|webserv " + nginxValue + " | " + webservValue + COLOR_RESET);
		return (false);
	}
	if (printOK)
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

function	compareStatus(nginxResponse, webservResponse, printOK)
{
	//status code + text
	const	nginxStatus = getStatus(nginxResponse);
	const	webservStatus = getStatus(webservResponse);

	return (verify("status : ", nginxStatus , webservStatus, printOK));
}

function	compareRedirection(nginxResponse, webservResponse, printOK)
{
	//redirection
	const nginxRedirection = nginxResponse.headers.get("Location");
	const webservRedirection = webservResponse.headers.get("Location");

	if (nginxRedirection !== null && webservRedirection !== null)
			return (verify("redirection : ", nginxRedirection, webservRedirection, printOK));
	return (true);
}

async function	compareBody(nginxResponse, webservResponse, printOK)
{
	//body
	const nginxBody = await nginxResponse.text();
	const webservBody = await webservResponse.text();

	if (!isError(nginxResponse) && !isError(webservResponse) // We don't have the same error pages
		&& nginxBody !== "" && webservBody !== ""
		&& !webservBody.includes("<a href=\"../\">") // We don't have the same autoIndex
		&& !nginxBody.includes("<a href=\"../\">")) // We don't have the same autoIndex
	{
		return (verify("body : ", nginxBody, webservBody, printOK));
	}
	return (true);
}

function	compareRequestEffect(target, nginxResponse, webservResponse, printOK)
{
	if ((nginxResponse.status == 201 && webservResponse.status == 201)
		|| (nginxResponse.status == 204 && webservResponse.status == 204))
	{
		if (nginxResponse.status == 201)
			console.log(COLOR_CYAN + "checking if the file has been created with the right body : " + COLOR_RESET)
		else
			console.log(COLOR_CYAN + "checking if the file has been deleted: " + COLOR_RESET)
		return (compareGoodRequests(target, "GET", null, [], printOK));
	}
	return (true);
}

async function	compareRequests(target, nginxResponse, webservResponse, printOK)
{
	let	succeed = true;

	succeed = compareStatus(nginxResponse, webservResponse, printOK) && succeed;
	succeed = compareRedirection(nginxResponse, webservResponse, printOK) && succeed;
	succeed = await compareBody(nginxResponse, webservResponse, printOK) && succeed;
	succeed = await compareRequestEffect(target, nginxResponse, webservResponse, printOK) && succeed;
	return (succeed);
}

export async function	compareGoodRequests(target, method, body, headers, printOK)
{
	console.log("target : " + COLOR_BLUE + target + COLOR_RESET);
	const nginxResponse = await makeRequest(nginxUrl + target, method, body, headers);
	const webservResponse = await makeRequest(webservUrl + target, method, body, headers);
	return (compareRequests(target, nginxResponse, webservResponse, printOK));
}

export async function	compareBadRequests(message, target, printOK)
{
	console.log("message : " + message);
	const nginxResponse = await makeRawRequest(nginxHost, nginxPort, message);
	const webservResponse = await makeRawRequest(webservHost, webservPort, message);
	return (compareRequests(target, nginxResponse, webservResponse, printOK));
}

export async function	compareGoodRequestWithValues(target, method, body, headers, statusCode, statusText)
{
	const webservResponse = await makeRequest(webservUrl + target, method, body, headers);
	const expectedResponse = {
		status: statusCode,
		statusText: statusText,
	};
	return (compareStatus(expectedResponse, webservResponse));
}

export async function	compareBadRequestWithValues(message, statusCode, statusText, printOK)
{
	const webservResponse = await makeRawRequest(webservHost, webservPort, message);
	const expectedResponse = {
		status: statusCode,
		statusText: statusText,
	};
	return (compareStatus(expectedResponse, webservResponse, printOK));
}
