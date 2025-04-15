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

FileFd::FileFd(const std::string& path, int flags, mode_t mode, EPollHandler& ePollHandler) :
	AFdData(openFile(path, flags, mode), false, ePollHandler)
{
}

FileFd::FileFd(const std::string& path, int flags, EPollHandler& ePollHandler) :
	AFdData(openFile(path, flags), false, ePollHandler)
{
}

FileFd::~FileFd()
{
}

void	FileFd::callback(uint32_t events)
{
	(void)events;
}
