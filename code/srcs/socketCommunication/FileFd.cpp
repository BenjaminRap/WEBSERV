#include "FileFd.hpp"	// for FILEFD
#include <cstdio>		// for tmpnam
#include <sys/types.h>	// for ssize_t

static int	openFile(const std::string& path, int flags, mode_t mode)
{
	int	fd = open(path.c_str(), flags, mode);

	if (fd == -1)
		throw FileFd::FileOpeningError();
	return (fd);
}

static int	openFile(const std::string& path, int flags)
{
	int	fd = open(path.c_str(), flags);

	if (fd == -1)
		throw FileFd::FileOpeningError();
	return (fd);
}

ssize_t		getFileSize(const std::string &filePath);

FileFd::FileFd(const std::string& path, int flags, mode_t mode) :
	AFdData(openFile(path, flags, mode), FILEFD),
	_fileSize(0)
{
	if (flags & O_CREAT & O_EXCL)
		return ;
	const ssize_t	fileSize = getFileSize(path);

	if (fileSize == -1)
		throw FileFd::FileOpeningError();
	_fileSize = fileSize;
}

FileFd::FileFd(const std::string& path, int flags) :
	AFdData(openFile(path, flags), FILEFD),
	_fileSize(0)
{
	if (flags & O_CREAT & O_EXCL)
		return ;
	const ssize_t	fileSize = getFileSize(path);

	if (fileSize == -1)
		throw FileFd::FileOpeningError();
	_fileSize = fileSize;
}

static int	openTemporaryFile(int rights)
{
	char* const	name = std::tmpnam(NULL);
	const int	fd = open(name, O_CREAT | rights);

	if (fd == -1)
		throw FileFd::FileOpeningError();
	std::remove(name);
	return (fd);
}

FileFd::FileFd(int rights) :
	AFdData(openTemporaryFile(rights), FILEFD),
	_fileSize(0)
{
}

FileFd::~FileFd()
{
}

void	FileFd::callback(uint32_t events)
{
	(void)events;
}

size_t	FileFd::getSize(void) const
{
	return (_fileSize);
}
