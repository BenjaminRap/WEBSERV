#include <fcntl.h>      // for open, O_CREAT, O_EXCL, O_RDONLY, O_WRONLY
#include <stdint.h>     // for uint32_t
#include <sys/types.h>  // for ssize_t, mode_t
#include <cstdio>       // for tmpnam, NULL, L_tmpnam, size_t
#include <new>          // for nothrow
#include <string>       // for string, basic_string

#include "AFdData.hpp"  // for AFdData, AFdDataChilds
#include "FileFd.hpp"   // for FileFd

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
	AFdData(openFile(path, flags, mode), FILE_FD),
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
	AFdData(openFile(path, flags), FILE_FD),
	_fileSize(0)
{
	if (flags & O_CREAT & O_EXCL)
		return ;
	const ssize_t	fileSize = getFileSize(path);

	if (fileSize == -1)
		throw FileFd::FileOpeningError();
	_fileSize = fileSize;
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


FileFd*	FileFd::getTemporaryFile(char (&name)[L_tmpnam], int rights)
{
	if (rights != O_RDONLY || rights != O_WRONLY)
		return (NULL);
	if (*name == '\0')
	{
		if (!std::tmpnam(name))
			return (NULL);
	}
	return (new (std::nothrow) FileFd(name, rights));
}
