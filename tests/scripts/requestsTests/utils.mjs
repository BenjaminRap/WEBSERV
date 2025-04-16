import * as childProcess from "child_process"
import { webservUrl, nginxUrl } from "./hosts.mjs"

export const COLOR_RESET = "\x1b[0m"
export const COLOR_RED = "\x1b[31m"
export const COLOR_GREEN = "\x1b[32m"
export const COLOR_BLUE = "\x1b[34m"
export const COLOR_MAGENTA = "\x1b[35m"
export const COLOR_CYAN = "\x1b[36m"

export function	printHeader(header)
{
	console.log("");
	const band = "|" + "-".repeat(header.length + 6) + "|";
	console.log(COLOR_MAGENTA + band + COLOR_RESET);
	console.log(COLOR_BLUE + "|   " + header + "   |" + COLOR_RESET);
	console.log(COLOR_MAGENTA + band + COLOR_RESET);
}

export function	exec(command)
{
	return (childProcess.spawnSync(command, { shell: true }));
}

export function	verifyServersAreRunning()
{
	{
		const result = exec("curl -I " + nginxUrl + " > /dev/null 2>&1")

		if (result.status != 0)
		{
			console.log("Nginx server not running, exiting")
			process.exit(0);
		}
	}

	{
		const result = exec("curl -I " + webservUrl + " > /dev/null 2>&1");

		if (result.status != 0)
		{
			console.log("WebServ server not running, exiting")
			process.exit(0);
		}
	}
}
