
#include <cstdlib>
#include <cstdio>

int	main(void)
{
	printf("Content-TYPe: text/html\r\n");
	printf("transfer-encODING: chunked\r\n");
	printf("location: /path/to/file\r\n");
	printf("\r\n");
	const char *chunkedBody =
		"7\r\n"
		"Mozilla\r\n"
		"9\r\n"
		"Developer\r\n"
		"7\r\n"
		"Network\r\n"
		"0\r\n"
		"\r\n";
	printf("%s", chunkedBody);
	return (EXIT_SUCCESS);
}
