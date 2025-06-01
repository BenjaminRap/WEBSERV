import net from "net";
import { printHeader, COLOR_GREEN, COLOR_RED, COLOR_RESET } from "./utils.mjs";

let failedTests = [];

async function runEpollinTimeoutTest() {
	printHeader("EPOLLIN Timeout (client n'envoie pas la requête complète)");
	return new Promise((resolve) => {
		const client = new net.Socket();
		let triggered = false;

		client.connect(8080, "localhost", () => {
			client.write("GET / HTTP/1.1\r\nHost: dummy\r\n");
		});

		client.on("end", () => {
			triggered = true;
			console.log(COLOR_GREEN + "[OK] " + COLOR_RESET);
			resolve(true);
		});
		client.on("error", () => {
			triggered = true;
			console.log(COLOR_GREEN + "[OK] (error)" + COLOR_RESET);
			resolve(true);
		});
		client.on("close", () => {
			if (!triggered) {
				console.log(COLOR_RED + "[KO]" + COLOR_RESET);
				failedTests.push("EPOLLIN");
				resolve(false);
			}
		});
	});
}

async function runEpolloutTimeoutTest() {
	printHeader("EPOLLOUT Timeout (client ne lit pas la réponse)");
	return new Promise((resolve) => {
		const client = new net.Socket();
		let triggered = false;

		client.connect(8080, "localhost", () => {
			client.write("GET / HTTP/1.1\r\nHost: dummy\r\n\r\n");
		});

		client.on("end", () => {
			triggered = true;
			console.log(COLOR_GREEN + "[OK] " + COLOR_RESET);
			resolve(true);
		});
		client.on("error", () => {
			triggered = true;
			console.log(COLOR_GREEN + "[OK] (error)" + COLOR_RESET);
			resolve(true);
		});
		client.on("close", () => {
			if (!triggered) {
				console.log(COLOR_RED + "[KO]" + COLOR_RESET);
				failedTests.push("EPOLLOUT");
				resolve(false);
			}
		});
	});
}

async function runTests() {
	await runEpollinTimeoutTest();
	await runEpolloutTimeoutTest();

	if (failedTests.length === 0)
		printHeader("Everything Done : " + COLOR_GREEN + "[OK] " + COLOR_RESET);
	else {
		printHeader("Everything Done : " + COLOR_RED + "[KO] " + COLOR_RESET);
		console.table(failedTests);
	}
}

async function run() {
	await runTests();
}

run();
