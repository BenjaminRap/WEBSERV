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
			method: method
		});
		console.log(response.status + " " + response.statusText);
	}
	catch (error) {
		console.error("fetch error");
	}
}

makeRequest();