#include <iostream>
#include <string>

std::string	test(int value)
{
	if (value == 0)
		return ("test");
	return ("truc");
}

void	verify(bool test)
{
	if (test)
		printf("\033[0;32mOK\033[0m\n");
	else
		printf("\033[0;31mKO\033[0m\n");
}

int	main(void)
{
	verify(test(0) == "test");
	verify(test(1) == "test");
	verify(test(1) == "truc");
}