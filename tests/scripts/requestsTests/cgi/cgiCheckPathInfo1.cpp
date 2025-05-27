#include <stdio.h>
#include <stdlib.h>
#include <string>

# ifndef EXPECTED_PATH_INFO
#  define EXPECTED_PATH_INFO "/test"
# endif

void	printHTTPVar(const char* name)
{
	printf("<li>%s: %s</li>\n", name, getenv(name));
}

int main(void)
{
	char*	pathInfo = getenv("PATH_INFO");

    printf("Content-Type: text/plain\r\n");
	if (pathInfo == NULL || std::string(pathInfo) != EXPECTED_PATH_INFO)
	{
		printf("StatUS: 500 INTERNAL SERVER ERROR\r\n\r\n");
		printf("Mauvais path_info, expected : '%s'", EXPECTED_PATH_INFO);
	}
	else
	{
		printf("StatUS: 200 OK\r\n\r\n");
		printf("path_info OK");
	}

    return 0;
}
