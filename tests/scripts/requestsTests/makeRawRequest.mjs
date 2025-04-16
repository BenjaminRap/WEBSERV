import { HTTPParser } from "/usr/local/lib/node_modules/http-parser-js/http-parser.js";
import * as net from "net";

export function	makeRawRequest(host, port, requestData)
{
	return (sendRawRequest(host, port, requestData)
		.then((response) => parseHttpResponse(response)));
}

function sendRawRequest(host, port, requestData)
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
	let	complete = false;
	let statusCode = "";
	let	statusText = "";

    // Callback pour récupérer les headers et les convertir d'un array a une map
    parser[HTTPParser.kOnHeadersComplete] = (response) => {
        headers = response.headers.reduce((map, key, index, array) => {
            if (index % 2 === 0)
				map[key] = array[index + 1];
            return (map);
        }, {});
		complete = true;
		statusCode = response.statusCode;
		statusText = response.statusMessage;
    };

    // Callback pour récupérer le corps
    parser[HTTPParser.kOnBody] = (chunk, start, len) => {
        body += chunk.toString('utf-8', start, start + len);
    };

    // Parser la réponse brute
    const buffer = Buffer.from(rawText, 'utf-8');
    parser.execute(buffer, 0, buffer.length);
	parser.finish();

	if (complete === false)
		throw new Error("Could not parse response !");

    // Créer l'objet Response de Fetch API
    return (new Response(body, {
		status: statusCode,
        statusText: statusText,
        headers: new Headers(headers),
    }));
}

