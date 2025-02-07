#include <sys/stat.h>
#include <cstring>
#include <vector>
#include <dirent.h>
#include <list>
#include <cerrno>
#include "GetRequest.hpp"

#define DIRE 1
#define FILE 2
#define NF 3
#define FORBIDEN -1
#define ERROR500 -2

void	delString(const std::string& toDel, std::string &str)
{
	size_t	found;
	size_t	len;

	len = toDel.length();
	found = str.find(toDel, 0);
	while (found != std::string::npos)
	{
		str.erase(found, len);
		found = str.find(toDel, 0);
	}
}

void	fixPath(std::string &path)
{
	size_t	found;
	size_t	foundBack;

	found = path.find("/../", 0);
	while (found != std::string::npos)
	{
		foundBack = path.find_last_of('/', found - 1);
		if (found == 0)
			path.erase(0, 3);
		else
			path.erase(foundBack, found - foundBack + 3);
		found = path.find("/../", 0);
	}
	delString("./", path);
	delString("//", path);
	if (path.empty())
		path = "/";
}

void checkType(std::string &path, GetRequest get)
{
	char lastChar = path[path.length() - 1];
	if (lastChar != '/')
	{
		path += "/"; // Can throw
		get.setResponse(301, "Moved Permanently",path);
	}
	else
		get.setUrl(path);
}

int	isDirOrFile(const std::string& path)
{
	struct stat	stats;

	if (path == "/")
		return (DIRE);
	std::memset(&stats, 0, sizeof(stats));
	if (stat(path.c_str(), &stats) == -1)
	{
		if (errno == EACCES)
			return (FORBIDEN);
		else
		return (NF);
	}
	if (S_ISDIR(stats.st_mode) == DIRE)
		return (DIRE);
	else
		return (FILE);
}

int	ls(const std::string& path, std::list<std::string> &lst)
{
	DIR							*dw;
	struct dirent				*res;

	dw = opendir(path.c_str());
	if (!dw)
	{
		if (errno == EACCES)
			return (FORBIDEN);
		else
			return (ERROR500);
	}
	while ((res = readdir(dw)))
		lst.push_back(res->d_name);
	closedir(dw);
	return (0);
}

std::string	buildPage(std::list<std::string>	&files, const std::string& path)
{
	std::string								result;
	std::list<std::string>::iterator		end;

	end = files.end();
	result = "<html>\n<head><title>Index of "; // Can throw
	result += path; // Can throw
	result += "</title></head>\n<body>\n<h1>Index of"; // Can throw
	result += path; // Can throw
	result += "</h1><hr><pre><a href=\"../\">../</a>\n"; // Can throw

	for (std::list<std::string>::iterator it = files.begin(); it != end; it++)
	{
		if (*it == ".." || *it == ".")
			continue ;
		result += "<a href=\""; // Can throw
		result += *it; // Can throw
		result += "\">"; // Can throw
		result += *it; // Can throw
		result += "</a>\n"; // Can throw
	}
	result += "</pre><hr></body>\n</html>"; // Can throw
	return (result);
}

bool	findIndex(GetRequest& get, const std::vector<std::string> &indexs)
{
	size_t		size;
	std::string	temp;

	size = indexs.size();
	for (unsigned long i = 0; i < size; i++)
	{
		temp = get.getUrl() + indexs[i];
		if (isDirOrFile(temp) == FILE)
		{
			get.setResponse(200, "OK", temp);
			return (true);
		}
	}
	return (false);
}


void	fixUrl(GetRequest& get, std::string& url)
{
	if (*url.begin() != '/')
		get.setResponse(400, "Bad Request", "Bad Request");
	else
	{
		fixPath(url);
		get.setUrl(url);
	}
}

void	autoIndexCase(GetRequest &get)
{
	std::list<std::string>	files;
	int						response;

	response = ls(get.getUrl(), files);
	if (response == FORBIDEN)
		get.setResponse(403, "Forbidden", get.getError(403));
	else if (response == ERROR500)
		get.setResponse(500, "Internal Server Error", get.getError(500));
	else
		get.file = buildPage(files, get.getUrl()); // Can throw as fuck
}

void	directoryCase(GetRequest &get)
{
	checkType(get.getUrl(), get); // Can throw
	if (get.code == 301)
		return;
	if (get.getIsRoot())
	{
		const std::vector<std::string>	&indexs = get.getIndexVec();
		if (findIndex(get, indexs))
			return ;
	}
	if (get.getAutoIndex())
		autoIndexCase(get);
	else
		get.setResponse(403, "Forbidden", get.getError(403));
}
