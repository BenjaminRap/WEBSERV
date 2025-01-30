const url = "http://localhost:8181/";

async function	makeRequest()
{
	try {
		const response = await fetch(url, {
			method: "GET"
		});
		if (!response.ok) {
			throw new Error(`Response status: ${response.status}`);
		}
		
		console.log(response.status + " " + response.statusText);
	} catch (error) {
		console.error(error.message);
	}
}

makeRequest();