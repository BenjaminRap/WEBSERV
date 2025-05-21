
#include <cstdlib>
#include <cstdio>

int	main(void)
{
	printf("Content-TYPe: text/html\r\n");
	printf("STATUS: 200 OK\r\n");
	printf("content-length: 8\r\n");
	printf("\r\n");
	printf("123456");
	return (EXIT_SUCCESS);
}
