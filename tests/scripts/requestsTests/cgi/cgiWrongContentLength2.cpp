
#include <cstdlib>
#include <cstdio>

int	main(void)
{
	printf("Content-TYPe: text/html\r\n");
	printf("content-LENGTH: a5\r\n");
	printf("STATUS: 200 OK\r\n");
	printf("\r\n");
	printf("12345");
	return (EXIT_SUCCESS);
}
