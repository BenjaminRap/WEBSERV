import { HTTPParser } from "/usr/local/lib/node_modules/http-parser-js/http-parser.js";
import * as net from "net";


let headers;
let body;
let	complete;
let statusCode;
let	statusText;

function	reset()
{
	headers = {};
	body = "";
	complete = false;
	statusCode = 0;
	statusText = "";
}

function	createResponse()
{
	if (body == "")
	{
		return (new Response(null, {
			status: statusCode,
			statusText: statusText,
			headers: new Headers(headers),
		}));
	}
	return (new Response(body, {
		status: statusCode,
		statusText: statusText,
		headers: new Headers(headers),
	}));
}

export function	makeRawRequest(host, port, requestData)
{
	reset();
	const httpParser = getHTTPParser();
	return (sendRawRequest(host, port, requestData, httpParser));
}

function sendRawRequest(host, port, requestData, httpParser)
{
    return new Promise((resolve, reject) => {
        const client = new net.Socket();

        client.connect(port, host, () => {
            client.write(requestData);
        });

        client.on('data', (data) => {
            const chunk = data.toString();
			const buffer = Buffer.from(chunk, 'utf-8');
			httpParser.execute(buffer, 0, buffer.length);
			if (complete == true)
				client.end();
        });

        client.on('end', () => {
			httpParser.finish();
			if (complete == true)
				return (resolve(createResponse()));
			return (reject("Could not finished"));
		});

        client.on('error', (err) => {
			httpParser.finish();
			return (resolve(createResponse()));
		});
    });
}

function getHTTPParser()
{
    const parser = new HTTPParser(HTTPParser.RESPONSE);

    // Callback pour récupérer les headers et les convertir d'un array a une map
    parser[HTTPParser.kOnHeadersComplete] = (response) => {
        headers = response.headers.reduce((map, key, index, array) => {
            if (index % 2 === 0)
				map[key] = array[index + 1];
            return (map);
        }, {});
		statusCode = response.statusCode;
		statusText = response.statusMessage;
    };

	parser[HTTPParser.kOnMessageComplete] = () => {
		complete = true;
	}

    // Callback pour récupérer le corps
    parser[HTTPParser.kOnBody] = (chunk, start, len) => {
        body += chunk.toString('utf-8', start, start + len);
    };

	return (parser);
}
