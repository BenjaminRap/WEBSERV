#include <dirent.h>               // for closedir, opendir, readdir, DIR
#include <stdint.h>               // for uint16_t
#include <sys/stat.h>             // for stat, S_ISDIR, S_ISREG
#include <cerrno>                 // for errno, EACCES, ENOENT, ENOTDIR
#include <cstring>                // for memset, size_t
#include <list>                   // for list, _List_iterator, operator!=
#include <new>                    // for bad_alloc
#include <string>                 // for basic_string, allocator, string
#include <vector>                 // for vector

#include "ARequestType.hpp"       // for DIRE, LS_FILE
#include "GetRequest.hpp"         // for GetRequest
#include "Status.hpp"             // for Status, StatusType
#include "requestStatusCode.hpp"  // for HTTP_FORBIDDEN, HTTP_INTERNAL_SERVE...

void	checkType(std::string &path, GetRequest &get)
{
	char lastChar = path[path.length() - 1];

	if (lastChar != '/')
	{
		path += "/";
		get.setRedirectionResponse(HTTP_MOVED_PERMANENTLY, path);
	}
	else
		get.setUrl(path);
}

uint16_t	isDirOrFile(const std::string& path)
{
	struct stat	stats;

	if (path == "/")
		return (DIRE);
	std::memset(&stats, 0, sizeof(stats));
	if (stat(path.c_str(), &stats) == -1)
	{
		if (errno == EACCES)
			return (HTTP_FORBIDDEN);
		else if (errno == ENOENT || errno == ENOTDIR)
			return (HTTP_NOT_FOUND);
		else
		{
			return (HTTP_INTERNAL_SERVER_ERROR);
		}
	}
	if (S_ISDIR(stats.st_mode))
		return (DIRE);
	else if (S_ISREG(stats.st_mode))
		return (LS_FILE);
	return (HTTP_NOT_FOUND);
}

uint16_t	ls(const std::string& path, std::list<std::string> &lst)
{
	DIR				*dw;
	struct dirent	*res;

	dw = opendir(path.c_str());
	if (!dw)
	{
		if (errno == EACCES)
			return (HTTP_FORBIDDEN);
		else
			return (HTTP_INTERNAL_SERVER_ERROR);
	}
	while ((res = readdir(dw)))
	{
		try {
			lst.push_back(res->d_name);
		}
		catch (std::bad_alloc &e)
		{
			closedir(dw);
			return (HTTP_INTERNAL_SERVER_ERROR);
		}
	}
	closedir(dw);
	return (HTTP_OK);
}

size_t	getBuildPagelength(std::list<std::string> &files, const std::string &path)
{
	size_t	length;

	length = 130 + 2 * path.size() + 1;
	for (std::list<std::string>::iterator it = files.begin(); it != files.end(); it++)
	{
		if (*it == ".." || *it == ".")
			continue ;
		length += 19 + it->size() * 2;
	}
	return (length);
}

std::string	buildPage(std::list<std::string> &files, const std::string& path)
{
	std::string								result;
	std::list<std::string>::iterator		end;

	result.reserve(getBuildPagelength(files, path));
	end = files.end();
	result = "<html>\n<head><title>Index of ";
	result += path;
	result += "</title></head>\n<body>\n<h1>Index of";
	result += path;
	result += "</h1><hr><pre><a href=\"../\">../</a>\n";

	for (std::list<std::string>::iterator it = files.begin(); it != end; it++)
	{
		if ((*it)[0] == '.')
			continue ;
		result += "<a href=\"";
		result += *it;
		result += "\">";
		result += *it;
		result += "</a>\n";
	}
	result += "</pre><hr></body>\n</html>";
	return (result);
}

bool	findIndex(GetRequest& get, const std::vector<std::string> &indexs)
{
	size_t		size;
	std::string	temp;
	uint16_t	ret;

	size = indexs.size();
	for (unsigned long i = 0; i < size; i++)
	{
		temp = get.getUrl() + indexs[i];
		ret = isDirOrFile(temp);
		if (ret == LS_FILE || ret == DIRE)
		{
			if (ret == DIRE)
				get.setRedirectionResponse(HTTP_MOVED_PERMANENTLY, temp + "/");
			else
				get.setRedirectionResponse(HTTP_OK, temp);
			return (true);
		}
	}
	return (false);
}

void	autoIndexCase(GetRequest &get)
{
	std::list<std::string>	files;
	uint16_t				response;

	response = ls(get.getUrl(), files);
	if (Status::isCodeOfType(response, STATUS_ERROR))
		get.setResponse(response);
	else
		get.setResponseWithAutoIndex(response, buildPage(files, get.getUrl()));
}

void	directoryCase(GetRequest &get)
{
	checkType(get.getUrl(), get);
	if (get.getCode() == HTTP_MOVED_PERMANENTLY)
		return;
	if (findIndex(get, get.getIndexs()))
		return ;
	if (get.getAutoIndex())
		autoIndexCase(get);
	else
		get.setResponse(HTTP_FORBIDDEN);
}
