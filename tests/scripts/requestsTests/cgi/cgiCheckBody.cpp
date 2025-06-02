#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <stdint.h>

#define EXPECTED_BODY "je suis le body"

int	main(void)
{
	char		buffer[100];
	uint16_t	code = 200;

	std::cerr << "before read" << std::endl;
	const int	rd = read(STDIN_FILENO, buffer, sizeof(buffer));
	std::cerr << "after read" << std::endl;
	if (rd == -1)
		code = 400;
	else
	{
		buffer[rd] = '\0';
		if (rd != (int)strlen(buffer)
			|| std::memcmp(buffer, EXPECTED_BODY, rd))
		{
			code = 400;
		}
	}

	printf("Content-TYPe: text/html\r\n");
	printf("content-length: 0\r\n");
	printf("status: %d\r\n", code);
	printf("\r\n");
	return (EXIT_SUCCESS);
}
