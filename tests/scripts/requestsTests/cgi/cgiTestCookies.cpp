#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

#ifndef SESSION_ID
# define SESSION_ID "aikhweui87"
#endif // !SESSION_ID

#ifndef EXPECTED_COOKIE
# define EXPECTED_COOKIE "aikhweui87"
#endif // !EXPECTED_COOKIE

int	main(void)
{
	const	char*	cookies = getenv("HTTP_COOKIE");

	if (cookies == NULL)
	{
		printf("Content-TYPe: text/html\r\n");
		printf("content-LENGTH: 0\r\n");
		printf("Set-COOKIE: " SESSION_ID "\r\n");
		printf("STATUS: 200 OK\r\n");
		printf("\r\n");
		return (EXIT_SUCCESS);
	}
	else if (std::string(cookies) == std::string(EXPECTED_COOKIE))
	{
		printf("Content-TYPe: text/html\r\n");
		printf("content-LENGTH: 0\r\n");
		printf("STATUS: 204 No Content\r\n");
		printf("\r\n");
		return (EXIT_SUCCESS);
	}
	printf("Content-TYPe: text/html\r\n");
	printf("content-LENGTH: 0\r\n");
	printf("STATUS: 400 Bad Request\r\n");
	printf("\r\n");
	return (EXIT_FAILURE);
}
