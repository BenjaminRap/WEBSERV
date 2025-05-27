#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	printf("StatUS: 200 OK\r\n");
	printf("content-type: text/html\r\n");
	printf("accept: text/html\r\n");
	printf("accept: application/xhtml+xml\r\n");
	printf("set-cookie: first_cookie=truc\r\n");
	printf("set-cookie: second_cookie=bidule\r\n");
	printf("cookie: first_cookie=bidule\r\n");
	printf("cookie: second_cookie=tuc\r\n");
	printf("cookie: third_cookie=quak; fourth_cookie=abla\r\n");
	printf("warning: attention\r\n");
	printf("warning: il\r\n");
	printf("warning: y\r\n");;
	printf("warning: a\r\n");
	printf("warning: un\r\n");
	printf("warning: probleme\r\n");
	printf("content-location: first\r\n");
	printf("content-location: second\r\n");
	printf("\r\n");

    return 0;
}
