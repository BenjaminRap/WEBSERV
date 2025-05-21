
#include <cstdlib>
#include <cstdio>

int	main(void)
{
	printf("transfer-encODING: chunked\r\n");
	printf("STATUS: 200 OK\r\n");
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
