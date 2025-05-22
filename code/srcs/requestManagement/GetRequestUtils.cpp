#include <dirent.h>               // for closedir, opendir, readdir, DIR
#include <stdint.h>               // for uint16_t
#include <sys/stat.h>             // for stat, S_ISDIR, S_ISREG
#include <cerrno>                 // for errno, EACCES, ENOENT, ENOTDIR
#include <cstring>                // for memset, size_t
#include <list>                   // for list, _List_iterator, operator!=
#include <new>                    // for bad_alloc
#include <string>                 // for basic_string, allocator, string

#include "ARequestType.hpp"       // for DIRE, LS_FILE
#include "GetRequest.hpp"         // for GetRequest
#include "Status.hpp"             // for Status, StatusType
#include "requestStatusCode.hpp"  // for HTTP_FORBIDDEN, HTTP_INTERNAL_SERVE...

bool	findIndex(ARequestType& req, const std::vector<std::string> &indexs);
void	checkType(std::string &path, ARequestType &req);

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
	std::string		fileName;

	fileName.reserve(257);
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
			fileName = res->d_name;
			if (res->d_type == DT_DIR)
				fileName.append("/");
			lst.push_back(fileName);
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

void	autoIndexCase(GetRequest &get)
{
	std::list<std::string>	files;
	uint16_t				response;

	response = ls(get.getPath(), files);
	if (Status::isCodeOfType(response, STATUS_ERROR))
		get.setResponse(response);
	else
		get.setResponseWithAutoIndex(response, buildPage(files, get.getPath()));
}

void	directoryCase(GetRequest &get)
{
	checkType(get.getPath(), get);
	if (get.getCode() == HTTP_MOVED_PERMANENTLY)
		return;
	if (findIndex(get, get.getIndexs()))
		return ;
	if (get.getAutoIndex())
		autoIndexCase(get);
	else
		get.setResponse(HTTP_FORBIDDEN);
}
