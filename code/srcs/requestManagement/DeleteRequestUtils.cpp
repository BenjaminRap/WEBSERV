#include <sys/stat.h>
#include <cstring>
#include <vector>
#include <dirent.h>
#include <list>
#include <cerrno>
#include <fstream>

#include "DeleteRequest.hpp"

#define DIRE 1
#define FILE 2
#define NF 3
#define FORBIDEN -1
#define ERROR500 -2


int							isDirOrFile(const std::string& path);
int							removeDirectory(const std::string& path, DeleteRequest &del);
void						delString(const std::string& toDel, std::string &str);
void						fixPath(std::string &path);
std::string					getParentPath(const std::string &path);


void checkEnd(std::string &path, DeleteRequest &del)
{
	char lastChar = path[path.length() - 1];
	if (lastChar != '/')
	{
		path += "/";
		del.setResponse(409, "Conflict");
	}
	else
		del.setUrl(path);
}

bool canWrite(const std::string &path)
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
	if (del.code == 409)
		del.setResponse(409, "Conflict");
	else if (!canWrite(path) || removeDirectory(path, del) != 0
			|| !canWrite(getParentPath(path)) || std::remove(path.c_str()) != 0)
		del.setResponse(500, "Internal Server Error");
	else
		del.setResponse(204, "No Content");
	return (del.code);
}

std::string	getParentPath(const std::string &path)
{
	size_t	found;
	std::string temp = path;

	found = temp.find_last_of('/', temp.length() - 2);
	temp.erase(found + 1, temp.length() - found);
	return (temp);
}

int	removeDirectory(const std::string &path, DeleteRequest &del)
{
	DIR							*dw;
	struct dirent				*res;

	dw = opendir(path.c_str());
	if (!dw)
		return (ERROR500);
	while ((res = readdir(dw)))
	{
		if (std::strcmp(res->d_name, ".") == 0 || std::strcmp(res->d_name, "..") == 0)
			continue ;
		else if (std::remove((path + res->d_name).c_str()) == -1)
		{
			if (isDirOrFile(path + res->d_name) == DIRE)
				directoryCase(path + res->d_name + "/", del);
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
		del.setResponse(403, "Forbidden");
	else if (std::remove(path.c_str()) != 0)
		del.setResponse(500, "Internal Server Error");
	else
		del.setResponse(204, "No Content");
	return (del.code);
}

