# include "Configuration.hpp"

void	ft_readfile(const char *path, std::string &buff)
{
	std::ifstream	file;

	file.open(path, file.in);
	if (file.fail())
		throw (Configuration::OpenFileException());
	if (!std::getline(file, buff, '\0'))
		throw (Configuration::ReadFileException());
}
