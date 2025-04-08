#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <unistd.h>

int main() {
    int epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        perror("epoll_create1");
        return EXIT_FAILURE;
    }

    int file_fd = open("test_file.txt", O_RDONLY | O_CREAT, 0666);
    if (file_fd == -1) {
        perror("open");
        close(epoll_fd);
        return EXIT_FAILURE;
    }

    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = file_fd;

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, file_fd, &event) == -1) {
        perror("epoll_ctl");
        printf("epoll ne fonctionne pas avec un fichier normal.\n");
    } else {
        printf("epoll fonctionne avec un fichier normal.\n");
    }

    close(file_fd);
    close(epoll_fd);
    return EXIT_SUCCESS;
}
