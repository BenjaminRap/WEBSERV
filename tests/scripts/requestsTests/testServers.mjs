import * as childProcess from "child_process"
import { HTTPParser } from "http-parser-js";

const protocol = "http"
const host = "localhost"
const nginxPort = 8181
const webservPort = 8080;

const nginxUrl = protocol + "://" + host + ":" + nginxPort;
const werbservUrl = protocol + "://" + host + ":" + webservPort;

const COLOR_RESET = "\x1b[0m"
const COLOR_RED = "\x1b[31m"
const COLOR_GREEN = "\x1b[32m"
const COLOR_BLUE = "\x1b[34m"
const COLOR_MAGENTA = "\x1b[35m"
const COLOR_CYAN = "\x1b[36m"

async function sendRawRequest(host, port, requestData)
{
    return new Promise((resolve, reject) => {
        const client = new net.Socket();
        let response = '';

        client.connect(port, host, () => {
            client.write(requestData);
        });

        client.on('data', (data) => {
            response += data.toString();
        });

        client.on('end', () => resolve(response));

        client.on('error', (err) => reject(err));
    });
}

function parseHttpResponse(rawText)
{
    const parser = new HTTPParser(HTTPParser.RESPONSE);
    let headers = {};
    let body = '';

    // Callback pour récupérer les headers et les convertir d'un array a une map
    parser[HTTPParser.kOnHeadersComplete] = (info) => {
        headers = info.headers.reduce((map, key, index, array) => {
            if (index % 2 === 0)
				map[key] = array[index + 1];
            return (map);
        }, {});
    };

    // Callback pour récupérer le corps
    parser[HTTPParser.kOnBody] = (chunk, start, len) => {
        body += chunk.toString('utf-8', start, start + len);
    };

    // Parser la réponse brute
    const buffer = Buffer.from(rawText, 'utf-8');
    parser.execute(buffer, 0, buffer.length);

    // Créer l'objet Response de Fetch API
    return new Response(body, {
        status: parser.info.statusCode,
        statusText: parser.info.statusMessage,
        headers: new Headers(headers),
    });
}

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
