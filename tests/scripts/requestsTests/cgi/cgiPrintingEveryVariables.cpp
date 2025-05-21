#include <stdio.h>
#include <stdlib.h>

void	printHTTPVar(const char* name)
{
	printf("<li>%s: %s</li>\n", name, getenv(name));
}

int main(void) {
    // Définir le type de contenu de la réponse
    printf("Content-Type: text/html\r\n\r\n");

    // Début du corps HTML
    printf("<!DOCTYPE html>\n");
    printf("<html lang=\"fr\">\n");
    printf("<head>\n");
    printf("<meta charset=\"UTF-8\">\n");
    printf("<title>Test CGI en C</title>\n");
    printf("</head>\n");
    printf("<body>\n");
    printf("<h1>Résultat du test CGI</h1>\n");
	printf("<ul>");
	printHTTPVar("SERVER_SOFTWARE");
	printHTTPVar("SERVER_NAME");
	printHTTPVar("GATEWAY_INTERFACE");
	printHTTPVar("SERVER_PROTOCOL");
	printHTTPVar("SERVER_PORT");
	printHTTPVar("REQUEST_METHOD");
	printHTTPVar("PATH_INFO");
	printHTTPVar("PATH_TRANSLATED");
	printHTTPVar("SCRIPT_NAME");
	printHTTPVar("QUERY_STRING");
	printHTTPVar("REMOTE_HOST");
	printHTTPVar("REMOTE_ADDR");
	printHTTPVar("AUTH_TYPE");
	printHTTPVar("REMOTE_USER");
	printHTTPVar("REMOTE_INDENT");
	printHTTPVar("CONTENT_TYPE");
	printHTTPVar("CONTENT_LENGTH");
	printHTTPVar("HTTP_ACCEPT");
	printHTTPVar("HTTP_ACCEPT_LANGUAGE");
	printHTTPVar("HTTP_USER_AGENT");
	printHTTPVar("HTTP_COOKIE");
	printHTTPVar("HTTP_REFERER");
	printf("</ul>");
    printf("</body>\n");
    printf("</html>\n");

    return 0;
}
