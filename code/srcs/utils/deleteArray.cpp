#include <cstddef> // for size_t

void	deleteArray(const char** array)
{
	if (array == NULL)
		return ;
	for (size_t i = 0; array[i] != NULL; i++)
	{
		delete[] array[i];
		array[i] = NULL;
	}
}
