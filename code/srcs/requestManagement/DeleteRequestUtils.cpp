#include <sys/stat.h>
#include <cstring>
#include <dirent.h>
#include <fstream>

#include "DeleteRequest.hpp"

#define DIRE 1
#define FILE 2
#define NF 3
#define FORBIDEN -1
#define ERROR500 -2


int				isDirOrFile(const std::string& path);
int				removeDirectory(const std::string& path, DeleteRequest &del);
std::string		getParentPath(const std::string &path);

void	checkEnd(std::string &path, DeleteRequest &del)
{
	char lastChar = path[path.length() - 1];
	if (lastChar != '/')
	{
		path += "/";
		del.setResponse(409, "Conflict", "Conflict");
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
	if (del.getCode() == 409)
		del.setResponse(409, "Conflict", "Conflict");
	else if (!canWrite(path) || removeDirectory(path, del) != 0
			|| !canWrite(getParentPath(path)) || std::remove(path.c_str()) != 0)
		del.setResponse(500, "Internal Server Error", "Internal Server Error");
	else
		del.setResponse(204, "No Content", "No Content");
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
		return (ERROR500);
	while ((res = readdir(dw)))
	{
		if (std::strcmp(res->d_name, ".") == 0 || std::strcmp(res->d_name, "..") == 0)
			continue ;
		temp = path + res->d_name;
		if (std::remove(temp.c_str()) == -1)
		{
			if (isDirOrFile(temp) == DIRE)
			{
				try
				{
					directoryCase(temp + "/", del);
				} catch (std::exception & e)
				{
					closedir(dw);
					return (ERROR500);
				}
			}
			else
			{
				closedir(dw);
				return (ERROR500);
			}
		}
	}
	closedir(dw);
	return (0);
}

int	fileCase(const std::string &path, DeleteRequest &del)
{
	if (!canWrite(getParentPath(path)))
		del.setResponse(403, "Forbidden", "Forbidden");
	else if (std::remove(path.c_str()) != 0)
		del.setResponse(500, "Internal Server Error", "Internal Server Error");
	else
		del.setResponse(204, "No Content", "No Content");
	return (del.getCode());
}

