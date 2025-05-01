#include "FileFd.hpp"

static	int	openFile(const std::string& path, int flags, mode_t mode)
{
	int	fd = open(path.c_str(), flags, mode);

	if (fd == -1)
		throw FileFd::FileOpeningError();
	return (fd);
}

static	int	openFile(const std::string& path, int flags)
{
	int	fd = open(path.c_str(), flags);

	if (fd == -1)
		throw FileFd::FileOpeningError();
	return (fd);
}

FileFd::FileFd(const std::string& path, int flags, mode_t mode) :
	AFdData(openFile(path, flags, mode), FILEFD)
{
}

FileFd::FileFd(const std::string& path, int flags) :
	AFdData(openFile(path, flags), FILEFD)
{
}

FileFd::~FileFd()
{
}

void	FileFd::callback(uint32_t events)
{
	(void)events;
}
