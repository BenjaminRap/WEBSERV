import { showDuration } from "./testOptions.mjs"
import { COLOR_BLUE, COLOR_RESET } from "./utils.mjs"

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
	if (showDuration)
		console.log(COLOR_BLUE + "duree : " +  (end - start).toFixed(3) + "ms" + COLOR_RESET);
	return (response);
}
