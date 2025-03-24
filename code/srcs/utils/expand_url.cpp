#include <sstream>
#include <string>
#include <iomanip>
#include <iostream>

int	uncode_url(std::string &url)
{
	size_t	i = url.find("%", 0);
	int	v;

	while (i != std::string::npos)
	{
		if (i < url.size() - 1 && (url[i + 1] >= '0' && url[i + 1] <= '7') && ((url[i + 2] >= '0' && url[i + 2] <= '9') || (url[i + 2] >= 'A' && url[i + 2] <= 'F')))
		{
			std::stringstream	s(url.substr(i + 1, 2));
			s >> std::hex >> v;
			url.erase(i, 3);
			url.insert(i, 1, static_cast<char>(v));
			i = url.find("%", i + 1);
		}
		else
			return (1);
	}
	return (0);
}