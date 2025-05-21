#include <stddef.h>  // for size_t
#include <string>    // for basic_string, string

void	splitInTwo(const std::string& str, char delimiter, std::string& firstPart, std::string& secondPart)
{
	size_t	pos = str.find(delimiter, 0);

	if (pos == std::string::npos)
	{
		firstPart = str;
		secondPart = "";
	}
	else
	{
		firstPart.append(str, 0, pos);
		secondPart.append(str, pos, std::string::npos);
	}
}

