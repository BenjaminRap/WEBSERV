const url = "http://localhost:9090/";

async function	makeRequest()
{
	try {
		const response = await fetch(url, {
			method: "GET",
			headers: {
				"Access-Control-Allow-Origin": "http://localhost:9090"
			}
		});
		if (!response.ok) {
			throw new Error(`Response status: ${response.status}`);
		}
		
		const json = await response.json();
		console.log(json);
	} catch (error) {
		console.error(error.message);
	}
}

makeRequest();