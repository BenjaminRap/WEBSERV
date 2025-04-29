#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <unistd.h>

int main() {
	const int	epollFd = epoll_create(1);

	if (epollFd == -1) {
		perror("epoll_create1");
		return EXIT_FAILURE;
	}

	const int	fileFd = open("test_file.txt", O_RDONLY | O_CREAT | O_NONBLOCK, 0666);

	if (fileFd == -1) {
		perror("open");
		close(epollFd);
		return EXIT_FAILURE;
	}

	struct epoll_event event;
	event.events = EPOLLIN;
	event.data.fd = fileFd;

	if (epoll_ctl(epollFd, EPOLL_CTL_ADD, fileFd, &event) == -1)
	{
		perror("epoll_ctl");
		printf("epoll ne fonctionne pas avec un fichier normal.\n");
	}
	else
		printf("epoll fonctionne avec un fichier normal.\n");

	close(fileFd);
	close(epollFd);
	return EXIT_SUCCESS;
}

// Bonjour, je pense avoir trouvé une erreur dans le sujet de webserv.
// Il est écrit dans le sujet (section III.1) : You must never do a read or a write operation without going through poll() (or
// equivalent).
// Et Epoll ne fonctionne pas avec des fichiers classiques, epoll_ctl utilisé avec epoll_ctl_add renvoie une erreur EBADF
// si on l'utilise avec un fichier classique.
// Voici un code pour le prouver :
// #include <stdio.h>
// #include <stdlib.h>
// #include <fcntl.h>
// #include <sys/epoll.h>
// #include <unistd.h>
//
// int main() {
// 	const int	epollFd = epoll_create(1);
//
// 	if (epollFd == -1) {
// 		perror("epoll_create1");
// 		return EXIT_FAILURE;
// 	}
//
// 	const int	fileFd = open("test_file.txt", O_RDONLY | O_CREAT, 0666);
//
// 	if (fileFd == -1) {
// 		perror("open");
// 		close(epollFd);
// 		return EXIT_FAILURE;
// 	}
//
// 	struct epoll_event event;
// 	event.events = EPOLLIN;
// 	event.data.fd = fileFd;
//
// 	if (epoll_ctl(epollFd, EPOLL_CTL_ADD, fileFd, &event) == -1)
// 	{
// 		perror("epoll_ctl");
// 		printf("epoll ne fonctionne pas avec un fichier normal.\n");
// 	}
// 	else
// 		printf("epoll fonctionne avec un fichier normal.\n");
//
// 	close(fileFd);
// 	close(epollFd);
// 	return EXIT_SUCCESS;
// }
// Dans le manuel d'epoll, il n'est pas décrit clairement si les fichiers classiques sont pris en compte ou non.
// Je n'ai donc pas pu trouver d'informations officielles là dessus, seulement  quelques sources: 
// https://stackoverflow.com/questions/8057892/epoll-on-regular-files
// https://darkcoding.net/software/linux-what-can-you-epoll/
// Notre groupe a quasiment finis tout le projet donc refaire l'architecture pourrait nous prendre
// beaucoup de temps.
// Je m'excuse si cette erreur vous a déjà été informée, ou si je me suis trompé et que ce
// n'est pas une erreur. 
// Aussi, si vous reconnaissez cette erreur comme telle mais que vous ne pouvez pas
// changer le sujet, pourriez vous envoyer un message sur discord s'il vous plait ?
// Ca ferait gagner beaucoup de temps à mon groupe et aux prochains.
//
// Aussi, cette règle: Checking the value of errno is strictly forbidden after a read or a write operation
// empêche d'utiliser epoll en edge-triggered, qui est une manière plus performante de gérer les
// operations d'IO. Ce n'est pas une erreur de sujet mais ça me semble quand même intéressant
// de pouvoir explorer cette option pour le server.
// Merci d'avoir lu ce message.
// Bonne journée.
// Benjamin Rappo (brappo).
