import * as childProcess from "child_process"
import { webservUrl, nginxUrl } from "./hosts.mjs"

export const COLOR_RESET = "\x1b[0m"
export const COLOR_RED = "\x1b[31m"
export const COLOR_GREEN = "\x1b[32m"
export const COLOR_BLUE = "\x1b[34m"
export const COLOR_MAGENTA = "\x1b[35m"
export const COLOR_CYAN = "\x1b[36m"


const characters ='ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789';
const charactersLength = characters.length;

export function	randomInt(min, max)
{
	return (Math.floor(Math.random() * (max - min) + min));
}

export function	generateString(min, max)
{
    let		result = '';
	const	length = randomInt(min, max);

    for (let i = 0; i < length; i++)
	{
		const	randomIndex = randomInt(0, charactersLength);

        result += characters[randomIndex];
    }
    return (result);
}

export function	printHeader(header)
{
	const band = "|" + "-".repeat(header.length + 6) + "|";
	console.log(COLOR_MAGENTA + band + COLOR_RESET);
	console.log(COLOR_BLUE + "|   " + header + "   |" + COLOR_RESET);
	console.log(COLOR_MAGENTA + band + COLOR_RESET);
}

export function	exec(command)
{
	return (childProcess.spawnSync(command, { shell: true }));
}

export async function	verifyServersAreRunning()
{
	await fetch(nginxUrl).catch(error => {
		console.log("Nginx server not running, exiting")
		process.exit(0);
	});

	await fetch(webservUrl).catch(error => {
		console.log("Webserv server not running, exiting")
		process.exit(0);
	});
}

export function	createChunkedRequest(target, headers, chunks, trailers)
{
	let message = "PUT " + target + " HTTP/1.1\r\n";
	headers.forEach((header) => {
		message += header + "\r\n";
	});
	message += "\r\n";
	chunks.forEach((chunk) => {
		message += chunk.length.toString(16) + "\r\n"
		message += chunk + "\r\n"
	});
	message += "0\r\n";
	trailers.forEach((trailer) => {
		message += trailer + "\r\n";
	});
	message += "\r\n";
	return (message);
}

