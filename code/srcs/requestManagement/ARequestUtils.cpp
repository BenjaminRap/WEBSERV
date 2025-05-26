#include <errno.h>                  // for errno, EACCES, EROFS
#include <stddef.h>                 // for size_t, NULL
#include <stdint.h>                 // for uint16_t
#include <sys/stat.h>               // for stat
#include <sys/types.h>              // for ssize_t
#include <unistd.h>                 // for access, X_OK
#include <string>                   // for basic_string, string
#include <utility>                  // for pair
#include <vector>                   // for vector
#include <algorithm>

#include "ARequestType.hpp"         // for ARequestType, DIRE, LS_FILE
#include "EMethods.hpp"             // for EMethods
#include "Route.hpp"                // for Route, SRedirection
#include "ServerConfiguration.hpp"  // for ServerConfiguration
#include "requestStatusCode.hpp"    // for HTTP_FORBIDDEN, HTTP_BAD_REQUEST
#include "socketCommunication.hpp"  // for checkError

uint16_t	isDirOrFile(const std::string& path);

uint16_t	isCgiExecutable(const std::string& path)
{
	const uint16_t	targetType = isDirOrFile(path);

	if (targetType == DIRE)
		return (HTTP_FORBIDDEN);
	if (targetType != LS_FILE)
		return (targetType);
	if (access(path.c_str(), X_OK) == -1)
	{
		if (errno == EACCES || errno == EROFS)
			return (HTTP_FORBIDDEN);
		return (HTTP_INTERNAL_SERVER_ERROR);
	}
	return (0);
}

bool	checkAllowMeth(const std::vector<EMethods>& accepted, EMethods meth)
{
	std::vector<EMethods>::const_iterator  it = std::find(accepted.begin(), accepted.end(), meth);
	return (it != accepted.end());
}

void	delString(const std::string &toDel, std::string &str)
{
	size_t	found;
	size_t	len;

	len = toDel.length();
	found = str.find(toDel, 0);
	while (found != std::string::npos)
	{
		str.erase(found, len - 1);
		found = str.find(toDel, 0);
	}
}

void	replaceUrl(const std::string &location, const std::string &root, std::string &url)
{
	if (root.empty())
		return ;
	url.replace(0, location.size(), root);
}

void	fixPath(std::string &path)
{
	size_t	found;
	size_t	foundBack;

	found = path.find("/../", 0);
	while (found != std::string::npos)
	{
		if (found == 0)
			path.erase(0, 3);
		else
		{
			foundBack = path.find_last_of('/', found - 1);
			path.erase(foundBack, found - foundBack + 3);
		}
		found = path.find("/../", 0);
	}
	delString("/./", path);
	delString("//", path);
	if (path.empty())
		path = "/";
}

void	fixUrl(ARequestType &req, std::string &url)
{
	if (*url.begin() != '/')
		req.setResponse(HTTP_BAD_REQUEST);
	else
	{
		fixPath(url);
		req.setPath(url);
	}
}

void	addRoot(ARequestType &req, const ServerConfiguration &config)
{
	const std::pair<const std::string, Route>*	route = config.getRouteFromPath(req.getPath());
	const std::string&							location = (route) ? route->first : "";
	const Route*								routeData = (route) ? &route->second : NULL;

	req.setRoute(routeData);
	if (!checkAllowMeth(req.getAcceptedMethods(), req.getMethod()))
	{
		req.setResponse(HTTP_METHOD_NOT_ALLOWED);
		return ;
	}
	if (route != NULL)
	{
		const std::string &redir = routeData->getRedirection().url;
		if (!redir.empty())
		{
			req.setResponseWithLocation(HTTP_MOVED_PERMANENTLY, redir, true);
			return ;
		}

	}
	replaceUrl(location, req.getRoot(), req.getPath());
}

ssize_t	getFileSize(const std::string &filePath)
{
	struct stat fileStat;

	const int	ret = stat(filePath.c_str(), &fileStat);
	
	if (checkError(ret, -1, "stat() : "))
		return (-1);
	return (fileStat.st_size);
}

bool	findIndex(ARequestType& req, const std::vector<std::string> &indexs)
{
	size_t		size;
	std::string	absolutePath;
	uint16_t	ret;

	size = indexs.size();
	for (unsigned long i = 0; i < size; i++)
	{
		absolutePath = req.getPath() + indexs[i];
		ret = isDirOrFile(absolutePath);
		if (ret == LS_FILE || ret == DIRE)
		{
			if (ret == DIRE)
			{
				req.setUrl(req.getUrl() + indexs[i] + "/");
				req.setResponseWithLocation(HTTP_MOVED_PERMANENTLY, absolutePath + "/", false);
			}
			else
			{
				req.setUrl(req.getUrl() + indexs[i]);
				req.setResponseWithLocation(HTTP_OK, absolutePath, false);
			}
			return (true);
		}
	}
	return (false);
}

void	checkType(std::string &path, ARequestType &req)
{
	char lastChar = path[path.length() - 1];

	if (lastChar != '/')
	{
		path += "/";
		req.setUrl(req.getUrl() + "/");
		req.setResponseWithLocation(HTTP_MOVED_PERMANENTLY, path, false);
	}
}
