#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>

#define BUFFER_SIZE 128
#define SERVER_PORT 8181

int
main(int argc, char *argv[])
{
	if (argc == 1)
	{
		std::cerr << "Not enough arguments, needs at least 1" << std::endl;
		return (EXIT_FAILURE);
	}
	int                 ret;
	int                 data_socket;
	ssize_t             r, w;
	struct sockaddr_in  addr;
	char                buffer[BUFFER_SIZE];

	/* Create local socket. */

	data_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (data_socket == -1) {
		perror("socket");
		exit(EXIT_FAILURE);
	}

	/*
	* For portability clear the whole structure, since some
	* implementations have additional (nonstandard) fields in
	* the structure.
	*/

	memset(&addr, 0, sizeof(addr));

	/* Connect socket to socket address. */

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(0);
	addr.sin_port = htons(SERVER_PORT);

	ret = connect(data_socket, (const struct sockaddr *) &addr,
					sizeof(addr));
	if (ret == -1) {
		perror("connect() : ");
		exit(EXIT_FAILURE);
	}

	/* Send arguments. */

	// while (true)
	// {
	// 	r = read(STDIN_FILENO, buffer, sizeof(buffer) - 1);
	// 	if (r <= 0)
	// 		break ;
	// 	buffer[r] = '\0';
	// 	w = write(data_socket, buffer, r + 1);
	// 	if (w == -1) {
	// 		perror("write() : ");
	// 		break;
	// 	}
	// }

	for (size_t i = 1; i < argc; i++)
	{
		w = write(data_socket, argv[i], strlen(argv[i]));
		if (w == -1) {
			perror("write() : ");
			break;
		}
	}

	/* Receive result. */

	do
	{
		r = read(data_socket, buffer, sizeof(buffer));
		if (r == -1) {
			perror("read");
			exit(EXIT_FAILURE);
		}

		/* Ensure buffer is 0-terminated. */

		buffer[sizeof(buffer) - 1] = 0;

		printf("%s\n", buffer);
		/* code */
	} while (r > 0);
	

	/* Close socket. */

	close(data_socket);

	exit(EXIT_SUCCESS);
}
