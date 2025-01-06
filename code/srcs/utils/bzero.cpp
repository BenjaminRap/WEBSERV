#include <cstddef>

void	bzero(char *value, size_t size)
{
	for (size_t i = 0; i < size; i++)
	{
		value[i] = '\0';
	}
}