#include <sys/stat.h>
#include <cstring>
#include <vector>
#include <dirent.h>
#include "GetRequest.hpp"

#define DIRE 1
#define FILE 2
#define NF 3

std::string	delString(const std::string& toDel, std::string str)
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
	return (str);
}

std::string	fixPath(std::string path)
{
	size_t	found;
	size_t	foundBack;

	found = path.find("../", 0);
	while (found != std::string::npos)
	{
		if (path[found - 1] == '/')
		{
			if (found > 2)
				foundBack = path.find_last_of('/', found - 2);
			else
				foundBack = path.find_last_of('/', found - 1);
			if (foundBack == found - 2)
				foundBack = path.find_last_of('/', found - 3);
			path.erase(foundBack, found - foundBack + 2);
		}
		found = path.find("../", 0);
	}
	path = delString("./", path);
	return (path);
}

bool	checkPath(std::string path)
{
	size_t	found;
	int		k;

	k = 0;
	found = path.find_first_of('/', 0);
	while (found != std::string::npos)
	{
		if (path[found + 1] == '\0')
			break ;
		else if (path[found + 1] == '.' && path[found + 2] == '.')
			k--;
		else if (path[found + 1] == '/' || path[found + 1] == '.')
			;
		else
			k++;
		if (k < 0)
			return (false);
		found = path.find_first_of('/', found + 1);
	}
	return (true);
}

std::string	checkType(const std::string& path, GetRequest& get)
{
	std::string	temp;

	temp = path;
	char lastChar = temp[temp.length() - 1];
	if (lastChar != '/')
	{
		temp = path + "/";
		get.setResponse(301, temp);
	}
	return (temp);
}

int	isDirOrFile(const std::string& path)
{
	struct stat	stats;

	if (path == "/")
		return (DIRE);
	std::memset(&stats, 0, sizeof(stats));
	if (stat(path.c_str(), &stats) == -1)
		return (NF);
	if (S_ISDIR(stats.st_mode) == DIRE)
		return (DIRE);
	else
		return (FILE);
}

std::vector<std::string>	ls(const std::string& path)
{
	DIR							*dw;
	struct dirent				*res;
	std::vector<std::string>	files;

	dw = opendir(path.c_str());
	if (!dw)
	{
		files.push_back("Error");
		return (files);
	}
	while ((res = readdir(dw)))
		files.push_back(res->d_name);
	closedir(dw);
	return (files);
}

std::string	buildPage(std::vector<std::string> files, const std::string& path)
{
	std::string	result;
	size_t		size;

	size = files.size() - 1;
	result = "<html>\n<head><title>Index of ";
	result += path;
	result += "</title></head>\n<body>\n<h1>Index of";
	result += path;
	result += "</h1><hr><pre><a href=\"../\">../</a>\n";
	for (unsigned long i = 0; i < size; i++)
	{
		if (files.empty())
			break;
		while ( i < size && (files[i] == ".." || files[i] == "."))
			i++;
		result+= "<a href=\"";
		result+= files[i];
		result += "\">";
		result+= files[i];
		result += "</a>\n";
	}
	result += "</pre><hr></body>\n</html>";
	return (result);
}

bool	findIndex(GetRequest& get, std::vector<std::string> indexs)
{
	size_t	size;

	size = indexs.size();
	for (unsigned long i = 0; i < size; i++)
	{
		if (isDirOrFile(get.getUrl() + indexs[i]) == FILE)
		{
			get.setResponse(200, get.getUrl() + indexs[i]);
			return (true);
		}
	}
	return (false);
}

void	fixUrl(GetRequest& get, const std::string& url)
{
	bool	isValid;

	isValid = checkPath(url);
	if (!isValid)
		get.setUrl("/");
	get.setUrl(fixPath(url));
}

void	autoIndexCase(GetRequest &get)
{
	std::vector<std::string>	files;

	files = ls(get.getUrl());
	if (files.empty())
		get.file = buildPage(files, get.getUrl());
	else if (files[0] == "Error")
	{
		get.setResponse(403, "Forbidden");
		return ;
	}
	else
		get.file = buildPage(files, get.getUrl());
}

void	directoryCase(GetRequest &get)
{
	get.setUrl(checkType(get.getUrl(), get));
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
		get.setResponse(403, "Forbidden");
}
