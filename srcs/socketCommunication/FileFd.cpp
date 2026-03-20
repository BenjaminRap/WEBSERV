#include <fcntl.h>      	// for open, O_CREAT, O_EXCL, O_WRONLY
#include <stdint.h>     	// for uint32_t
#include <sys/types.h>  	// for ssize_t, mode_t
#include <cstdio>       	// for tmpnam, NULL, L_tmpnam, size_t
#include <string>       	// for basic_string, string

#include "AFdData.hpp"  	// for AFdData, AFdDataChilds
#include "FileFd.hpp"   	// for FileFd
#include "ContentTypes.hpp" // for ContentTypes

static int	openFile(const char* path, int flags, mode_t mode)
{
	int	fd = open(path, flags, mode);

	if (fd == -1)
		throw FileFd::FileOpeningError();
	return (fd);
}

static int	openFile(const char* path, int flags)
{
	int	fd = open(path, flags);

	if (fd == -1)
		throw FileFd::FileOpeningError();
	return (fd);
}

ssize_t		getFileSize(const std::string &filePath);

FileFd::FileFd(const std::string& path, int flags, mode_t mode) :
	AFdData(openFile(path.c_str(), flags, mode), FILE_FD, false),
	_fileSize(0),
	_contentType(ContentTypes::getFileContentType(path))
{
	if (flags & O_CREAT & O_EXCL)
		return ;
	const ssize_t	fileSize = getFileSize(path);

	if (fileSize == -1)
		throw FileFd::FileOpeningError();
	_fileSize = fileSize;
}

FileFd::FileFd(const std::string& path, int flags) :
	AFdData(openFile(path.c_str(), flags), FILE_FD, false),
	_fileSize(0),
	_contentType(ContentTypes::getFileContentType(path))
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


const std::string&	FileFd::getContentType() const
{
	return (_contentType);
}

FileFd*	FileFd::getTemporaryFile(char (&name)[L_tmpnam])
{
	if (!std::tmpnam(name))
		return (NULL);
	try
	{
		return (new FileFd(name, O_WRONLY | O_CREAT | O_EXCL, 0600));
	}
	catch (FileFd::FileOpeningError& e)
	{
		return (NULL);
	}
}
