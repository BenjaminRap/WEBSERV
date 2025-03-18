#include <dirent.h>               // for closedir, dirent, opendir, readdir
#include <sys/stat.h>             // for stat, S_IWGRP, S_IWOTH
#include <cstdio>                 // for remove, size_t
#include <cstring>                // for strcmp, memset
#include <exception>              // for exception
#include <string>                 // for allocator, basic_string, string

#include "ARequestType.hpp"       // for DIRE
#include "DeleteRequest.hpp"      // for DeleteRequest
#include "requestStatusCode.hpp"  // for HTTP_INTERNAL_SERVER_ERROR, HTTP_CO...

int				isDirOrFile(const std::string &path);
int				removeDirectory(const std::string &path, DeleteRequest &del);
std::string		getParentPath(const std::string &path);

void	checkEnd(std::string &path, DeleteRequest &del)
{
	char lastChar = path[path.length() - 1];
	if (lastChar != '/')
	{
		path += "/";
		del.setResponse(HTTP_CONFLICT);
	}
	else
		del.setUrl(path);
}

bool	canWrite(const std::string &path)
{
	struct stat	stats;

	std::memset(&stats, 0, sizeof(stats));
	if (stat(path.c_str(), &stats) == -1)
		return (false);
	if (!(stats.st_mode & S_IWGRP))
		return (false);
	if (!(stats.st_mode & S_IWOTH))
		return (false);
	return (true);
}

int	directoryCase(const std::string &path, DeleteRequest &del)
{
	checkEnd(del.getUrl(), del);
	if (del.getCode() == HTTP_CONFLICT)
		del.setResponse(HTTP_CONFLICT);
	else if (!canWrite(path) || removeDirectory(path, del) != 0
			|| !canWrite(getParentPath(path)) || std::remove(path.c_str()) != 0)
		del.setResponse(HTTP_INTERNAL_SERVER_ERROR);
	else
		del.setResponse(HTTP_NO_CONTENT);
	return (del.getCode());
}

std::string	getParentPath(const std::string &path)
{
	size_t		found;
	std::string	temp;

	temp = path;
	found = temp.find_last_of('/', temp.length() - 2);
	temp.erase(found + 1, temp.length() - found);
	return (temp);
}

int	removeDirectory(const std::string &path, DeleteRequest &del)
{
	DIR				*dw;
	struct dirent	*res;
	std::string		temp;

	dw = opendir(path.c_str());
	if (!dw)
		return (HTTP_INTERNAL_SERVER_ERROR);
	while ((res = readdir(dw)))
	{
		try
		{
			if (std::strcmp(res->d_name, ".") == 0 || std::strcmp(res->d_name, "..") == 0)
				continue ;
			temp = path + res->d_name;
			if (std::remove(temp.c_str()) == -1)
			{
				if (isDirOrFile(temp) == DIRE)
					directoryCase(temp + "/", del);
				else
				{
					closedir(dw);
					return (HTTP_INTERNAL_SERVER_ERROR);
				}
			}
		}
		catch (std::exception & e)
		{
			closedir(dw);
			return (HTTP_INTERNAL_SERVER_ERROR);
		}
	}
	closedir(dw);
	return (HTTP_OK);
}

int	fileCase(const std::string &path, DeleteRequest &del)
{
	if (!canWrite(getParentPath(path)))
		del.setResponse(HTTP_FORBIDDEN);
	else if (std::remove(path.c_str()) != 0)
		del.setResponse(HTTP_INTERNAL_SERVER_ERROR);
	else
		del.setResponse(HTTP_NO_CONTENT);
	return (del.getCode());
}

