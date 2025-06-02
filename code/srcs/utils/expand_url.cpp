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

void	encode_url(std::string &url)
{
	size_t	i = url.find_first_of("!#$&'()*+,:;=?@[] \"%-<>\\^_`{|}~\r\n\t", 0);
	int	v;

	while (i != std::string::npos)
	{
		v = static_cast<int>(url[i]);
		std::stringstream	s;
		s << "%" << std::setw(2) << std::setfill('0') << std::hex << std::uppercase << v;
		url.erase(i);
		url.insert(i, s.str());
		i = url.find_first_of("!#$&'()*+,:;=?@[] \"%-<>\\^_`{|}~\r\n\t", i + 3);
	}
}

int main(int argc, char **argv)
{
	if (argc == 2)
	{
		std::string s(argv[1]);
		uncode_url(s);
		std::cout << s << std::endl;
	}
	else
		std::cout << "Wrong number of arguments" << std::endl;
}