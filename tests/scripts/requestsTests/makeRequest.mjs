export async function	makeRequest(url, method, body, headers)
{
	const start = performance.now();
	const	response = await fetch(url, {
		method: method,
		redirect: 'manual',
		body: body,
		headers: headers
	});
	const end = performance.now();
	console.log("duree : " +  (end - start).toFixed(3) + "ms");
	return (response);
}
