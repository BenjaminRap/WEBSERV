const fetch = require("node-fetch");

if (process.argv.length != 4)
{
	console.error("Not enough parameters");
	return ;
}

const host = "http://localhost:8181";
const url = process.argv[2];
const method = process.argv[3];

async function	makeRequest()
{
	try
	{
		const response = await fetch(host + url, {
			method: method,
			redirect: 'manual'
		});
		console.log(response.status + " " + response.statusText);
	}
	catch (error) {
		console.error("fetch error");
		console.error("Fetch error:", error);
        console.error("Error message:", error.message);
        console.error("Error name:", error.name);
	}
}

makeRequest();
