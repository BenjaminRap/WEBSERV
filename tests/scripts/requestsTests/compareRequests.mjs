import { exec, COLOR_RESET, COLOR_RED, COLOR_GREEN, COLOR_BLUE, COLOR_MAGENTA, COLOR_CYAN } from "./utils.mjs"
import { makeRequest } from "./makeRequest.mjs"
import { makeRawRequest } from "./makeRawRequest.mjs"
import { nginxHost, webservHost, nginxPort, webservPort, nginxUrl, webservUrl } from "./hosts.mjs"
import { printOK, showDuration } from "./testOptions.mjs"

function	verify(prefix, nginxValue, webservValue)
{
	if (nginxValue !== webservValue)
	{
		console.log(prefix + COLOR_RED + "[KO] nginx|webserv '" + nginxValue + "' | '" + webservValue + "'" + COLOR_RESET);
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

function	compareStatus(nginxResponse, webservResponse)
{
	//status code + text
	const	nginxStatus = getStatus(nginxResponse);
	const	webservStatus = getStatus(webservResponse);

	return (verify("status : ", nginxStatus , webservStatus));
}

function	compareRedirection(nginxResponse, webservResponse)
{
	//redirection
	let	nginxRedirection = nginxResponse.headers.get("Location")?.slice(0);
	let	webservRedirection = webservResponse.headers.get("Location")?.slice(0);

	nginxRedirection = nginxRedirection?.replace(nginxPort.toString(), "port")
	webservRedirection = webservRedirection?.replace(webservPort.toString(), "port")

	nginxRedirection = nginxRedirection?.replace(nginxHost, "host")
	webservRedirection = webservRedirection?.replace(webservHost, "host")

	if (nginxRedirection !== null || webservRedirection !== null)
	{
		return (verify("redirection: ",  nginxRedirection, webservRedirection));
	}
	return (true);
}

function	compareAutoIndexBody(nginxBody, webservBody)
{
	let	posNginx = nginxBody.indexOf("<a href=");
	let	posWebserv = webservBody.indexOf("<a href=");
	let	nginxFiles = [];
	let	webservFiles = [];

	while (posNginx != -1 && posWebserv != -1)
	{
		let	nginxFile = nginxBody.slice(posNginx, nginxBody.indexOf("/a>", posNginx));
		let	webservFile = webservBody.slice(posWebserv, webservBody.indexOf("/a>", posWebserv));

		nginxFiles.push(nginxFile);
		webservFiles.push(webservFile);

		posNginx = nginxBody.indexOf("<a href=", posNginx + 8);
		posWebserv = webservBody.indexOf("<a href=", posWebserv + 8);
	}
	if (posNginx != posWebserv || nginxFiles.length != webservFiles.length)
		return (false);
	for (let i = 0; i < nginxFiles.length; i++)
	{
		if (!webservFiles.find((element) => element == nginxFiles[i]))
			return (false);
	}
	return (true);
}

async function	compareBody(nginxResponse, webservResponse)
{
	//body
	let nginxBody = await nginxResponse.text();
	let webservBody = await webservResponse.text();

	nginxBody = nginxBody.replace(/nginx\/[0-9]+(\.([0-9]+))*/, "serv/version");
	webservBody = webservBody.replace(/webserv\/[0-9]+(\.([0-9]+))*/, "serv/version");


	if (nginxBody.includes("<hr><pre><a href=\"../\">../</a>")
		&& webservBody.includes("<hr><pre><a href=\"../\">../</a>"))
	{
		let	bodyEquals = compareAutoIndexBody(nginxBody, webservBody);

		if (!bodyEquals)
			console.log("auto index body" + COLOR_RED + "[KO] nginx|webserv '" + nginxBody + "' | '" + webservBody + "'" + COLOR_RESET);
		else if (printOK)
			console.log("auto index body" + COLOR_GREEN + "[OK] nginx|webserv '" + nginxBody + "' | '" + webservBody + "'" + COLOR_RESET);
		return (bodyEquals);
	}
	return (verify("body : ", nginxBody, webservBody));
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
	succeed = await compareRequestEffect(target, nginxResponse, webservResponse) && succeed;
	return (succeed);
}

function	checkHeaders(webservHeaders, expectedHeaders)
{
	let	succeed = true;

	expectedHeaders.forEarch((element) => {
		if (webservHeaders.get(element.key) != element.value)
		{
			succeed = false;
			if (printOK)
				console.log("Missing header, expected:" + element);
		}
	});
	return (succeed);
}

export async function	compareGoodRequests(target, method, body, headers, expectedHeaders)
{
	try
	{
		let	succeed = true;

		if (showDuration)
			console.log(COLOR_BLUE + "nginx" + COLOR_RESET);
		const nginxResponse = await makeRequest(nginxUrl + target, method, body, headers);
		if (showDuration)
			console.log(COLOR_BLUE + "webserv" + COLOR_RESET);
		const webservResponse = await makeRequest(webservUrl + target, method, body, headers);
		succeed =  compareRequests(target, nginxResponse, webservResponse) && succeed;
		if (expectedHeaders != undefined)
			succeed = checkHeaders(webservResponse.headers, expectedHeaders) && succeed;
		return (succeed);
	}
	catch (error)
	{
		console.log(error);	
		return (false);
	}
}

export async function	compareBadRequests(message, target, expectedHeaders)
{
	try
	{
		let	succeed = true;

		if (showDuration)
			console.log(COLOR_BLUE + "nginx" + COLOR_RESET);
		const nginxResponse = await makeRawRequest(nginxHost, nginxPort, message);
		if (showDuration)
			console.log(COLOR_BLUE + "webserv" + COLOR_RESET);
		const webservResponse = await makeRawRequest(webservHost, webservPort, message);
		succeed = compareRequests(target, nginxResponse, webservResponse) && succeed;
		if (expectedHeaders != undefined)
			succeed = checkHeaders(webservResponse.headers, expectedHeaders) && succeed;
		return (succeed);
	}
	catch (error)
	{
		console.log(error);
		return (false);
	}
}

export async function	compareGoodRequestWithValues(target, method, body, headers, statusCode, statusText, expectedHeaders)
{
	try
	{
		let	succeed = true;

		if (showDuration)
			console.log(COLOR_BLUE + "webserv" + COLOR_RESET);
		const webservResponse = await makeRequest(webservUrl + target, method, body, headers);
		const expectedResponse = {
			status: statusCode,
			statusText: statusText,
		};
		succeed = compareStatus(expectedResponse, webservResponse) && succeed;
		if (expectedHeaders != undefined)
			succeed = checkHeaders(webservResponse.headers, expectedHeaders) && succeed;
		return (succeed);
	}
	catch (error)
	{
		console.log(error);
		return (false);
	}
}

export async function	compareBadRequestWithValues(message, statusCode, statusText, expectedHeaders)
{
	try
	{
		let	succeed = true;

		if (showDuration)
			console.log(COLOR_BLUE + "webserv" + COLOR_RESET);
		const webservResponse = await makeRawRequest(webservHost, webservPort, message);
		const expectedResponse = {
			status: statusCode,
			statusText: statusText,
		};
		succeed = compareStatus(expectedResponse, webservResponse) && succeed;
		if (expectedHeaders != undefined)
			succeed = checkHeaders(webservResponse.headers, expectedHeaders) && succeed;
		return (succeed);
	}
	catch (error)
	{
		console.log(error);
		return (false);
	}
}
