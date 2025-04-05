export function	makeRequest(url, method, body, headers)
{
	return (fetch(url, {
		method: method,
		redirect: 'manual',
		body: body,
		headers: headers
	}));
}
