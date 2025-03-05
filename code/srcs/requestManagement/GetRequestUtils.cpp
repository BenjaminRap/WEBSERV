#include <sys/stat.h>
#include <cstring>
#include <vector>
#include <dirent.h>
#include <list>
#include <cerrno>
#include "GetRequest.hpp"

void	checkType(std::string &path, GetRequest &get)
{
	char lastChar = path[path.length() - 1];

	if (lastChar != '/')
	{
		path += "/";
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
		else if (errno == ENOENT || errno == ENOTDIR)
			return (NF);
		else
			return (ERROR500);
	}
	if (S_ISDIR(stats.st_mode) == DIRE)
		return (DIRE);
	else
		return (LS_FILE);
}

int	ls(const std::string& path, std::list<std::string> &lst)
{
	DIR				*dw;
	struct dirent	*res;

	dw = opendir(path.c_str());
	if (!dw)
	{
		if (errno == EACCES)
			return (FORBIDEN);
		else
			return (ERROR500);
	}
	while ((res = readdir(dw)))
	{
		try {
			lst.push_back(res->d_name);
		}
		catch (std::bad_alloc &e)
		{
			closedir(dw);
			return (ERROR500);
		}
	}
	closedir(dw);
	return (0);
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

	size = indexs.size();
	for (unsigned long i = 0; i < size; i++)
	{
		temp = get.getUrl() + indexs[i];
		if (isDirOrFile(temp) == LS_FILE)
		{
			get.setResponse(200, "OK", temp);
			return (true);
		}
	}
	return (false);
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
		get.setResponse(200, "OK", buildPage(files, get.getUrl()));
}

void	directoryCase(GetRequest &get)
{
	checkType(get.getUrl(), get);
	if (get.getCode() == 301)
		return;
	if (get.getIsRoute())
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
