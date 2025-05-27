#include <stdio.h>
#include <stdlib.h>
#include <string>

# ifndef EXPECTED_PATH_INFO
#  define EXPECTED_PATH_INFO "/test"
# endif //!EXPECTED_PATH_INFO

# ifndef EXPECTED_SCRIPT_NAME
#  define EXPECTED_SCRIPT_NAME "/cgi/cgiCheckPathInfo3.cgi"
# endif //!EXPECTED_SCRIPT_NAME

# ifndef EXPECTED_QUERY_STRING
#  define EXPECTED_QUERY_STRING "/cgi/cgiCheckPathInfo3.cgi"
# endif //!EXPECTED_QUERY_STRING

void	printHTTPVar(const char* name)
{
	printf("<li>%s: %s</li>\n", name, getenv(name));
}

int main(void)
{
	char*	pathInfo = getenv("PATH_INFO");
	char*	scriptName = getenv("SCRIPT_NAME");
	char*	queryString = getenv("QUERY_STRING");

    printf("Content-Type: text/plain\r\n");
	if (pathInfo == NULL || std::string(pathInfo) != EXPECTED_PATH_INFO)
	{
		printf("StatUS: 500 INTERNAL SERVER ERROR\r\n\r\n");
		printf("Invalid path_info, we got : %s, expected : '%s'", pathInfo, EXPECTED_PATH_INFO);
		return (0);
	}
	if (scriptName == NULL || std::string(scriptName) != EXPECTED_SCRIPT_NAME)
	{
		printf("StatUS: 500 INTERNAL SERVER ERROR\r\n\r\n");
		printf("Invalid script name, we got : %s, expected : '%s'", scriptName, EXPECTED_SCRIPT_NAME);
		return (0);
	}
	if (queryString != NULL)
	{
		if (*EXPECTED_QUERY_STRING == '\0')
		{
			printf("StatUS: 500 INTERNAL SERVER ERROR\r\n\r\n");
			printf("Query string should be NULL, we got : %s", queryString);
			return (0);
		}
	  	if (std::string(queryString) != EXPECTED_QUERY_STRING)
		{
			printf("StatUS: 500 INTERNAL SERVER ERROR\r\n\r\n");
			printf("Invalid Query string, we got %s, expected : %s", queryString, EXPECTED_QUERY_STRING);
			return (0);
		}
	}
	else if (*EXPECTED_QUERY_STRING != '\0')
	{
			printf("StatUS: 500 INTERNAL SERVER ERROR\r\n\r\n");
			printf("Query string is NULL, expected : '%s'", EXPECTED_QUERY_STRING);
			return (0);
	}
	printf("StatUS: 200 OK\r\n\r\n");
	printf("path_info OK");

    return 0;
}
