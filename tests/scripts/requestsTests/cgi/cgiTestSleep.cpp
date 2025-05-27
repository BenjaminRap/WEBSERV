#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(void) {
    // Définir le type de contenu de la réponse
    printf("Content-Type: text/html\r\n\r\n");
	fflush(stdout);
	usleep(100000);

    // Début du corps HTML
    printf("<!DOCTYPE html>\n");
	fflush(stdout);
	usleep(100000);
    printf("<html lang=\"fr\">\n");
	fflush(stdout);
	usleep(100000);
    printf("<head>\n");
	fflush(stdout);
	usleep(100000);
    printf("<meta charset=\"UTF-8\">\n");
	fflush(stdout);
	usleep(100000);
    printf("<title>Test CGI en C</title>\n");
	fflush(stdout);
	usleep(100000);
    printf("</head>\n");
	fflush(stdout);
	usleep(100000);
    printf("<body>\n");
	fflush(stdout);
	usleep(100000);
    printf("<h1>Résultat du test CGI</h1>\n");
	fflush(stdout);

    // Vérifier la variable d'environnement SERVER_PROTOCOL
    char *protocol = getenv("SERVER_PROTOCOL");
    if (protocol != NULL) {
        printf("<p>Protocole HTTP détecté : <strong>%s</strong></p>\n", protocol);
        // Vérifier si le protocole contient "HTTP/" (base du protocole HTTP)
        if (strstr(protocol, "HTTP/") != NULL) {
            printf("<p style=\"color: green;\">Le protocole semble respecter le format HTTP.</p>\n");
        } else {
            printf("<p style=\"color: red;\">Attention : Le protocole ne semble pas respecter le format HTTP standard.</p>\n");
        }
    } else {
        printf("<p style=\"color: red;\">Erreur : La variable d'environnement SERVER_PROTOCOL n'est pas définie.</p>\n");
    }

    // Vérifier la variable d'environnement REQUEST_METHOD
    char *method = getenv("REQUEST_METHOD");
    if (method != NULL) {
        printf("<p>Méthode de requête détectée : <strong>%s</strong></p>\n", method);
        // Vérifier si la méthode est une méthode HTTP courante (GET, POST, etc.)
        if (strcmp(method, "GET") == 0 || strcmp(method, "POST") == 0 ||
            strcmp(method, "PUT") == 0 || strcmp(method, "DELETE") == 0 ||
            strcmp(method, "HEAD") == 0 || strcmp(method, "OPTIONS") == 0) {
            printf("<p style=\"color: green;\">La méthode de requête est une méthode HTTP courante.</p>\n");
        } else {
            printf("<p style=\"color: orange;\">La méthode de requête '%s' n'est pas une méthode HTTP standard courante.</p>\n", method);
        }
    } else {
        printf("<p style=\"color: red;\">Erreur : La variable d'environnement REQUEST_METHOD n'est pas définie.</p>\n");
    }

    // Vérifier la variable d'environnement CONTENT_TYPE (si la méthode est POST ou PUT)
    if (method != NULL && (strcmp(method, "POST") == 0 || strcmp(method, "PUT") == 0)) {
        char *contentType = getenv("CONTENT_TYPE");
        if (contentType != NULL) {
            printf("<p>Type de contenu de la requête : <strong>%s</strong></p>\n", contentType);
            // Vérifier si le Content-Type est défini (important pour les requêtes avec corps)
            if (strlen(contentType) > 0) {
                printf("<p style=\"color: green;\">L'en-tête Content-Type est présent pour une requête %s.</p>\n", method);
            } else {
                printf("<p style=\"color: orange;\">Attention : L'en-tête Content-Type est vide pour une requête %s.</p>\n", method);
            }
        } else {
            printf("<p style=\"color: orange;\">L'en-tête Content-Type n'est pas défini pour une requête %s.</p>\n", method);
        }
    }

    printf("</body>\n");
	fflush(stdout);
	usleep(100000);
    printf("</html>\n");
	fflush(stdout);
	usleep(100000);

    return 0;
}
