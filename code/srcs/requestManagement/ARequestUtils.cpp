#include "ARequestType.hpp"

bool	checkAllowMeth(const Route &route, EMethods meth)
{
	const std::vector<EMethods>	&meths = route.getAcceptedMethods();
	size_t						len;

	len = meths.size();
	if (len == 0)
		return (true);
	for (size_t i = 0; i < len ; i++)
	{
		if (meths[i] == meth)
			return (true);
	}
	return (false);
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

void	buildNewURl(std::string root, std::string &url)
{
	if (!root.empty() && root[root.size() - 1] == '/')
		root.erase(root.size() - 1);
	url.insert(0, root);
}

void	replaceUrl(const std::string &location, const std::string &root, std::string &url)
{
	size_t found;

	if (root.empty())
		return ;
	found = url.find(location);
	while (found != std::string::npos)
	{
		url.replace(found, location.length(), root);
		found = url.find(location, found + root.length());
	}
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
		req.setResponse(400, "Bad Request", "Bad Request");
	else
	{
		fixPath(url);
		req.setUrl(url);
	}
}

void	addRoot(ARequestType &get, const ServerConfiguration &config)
{
	const Route	*temp = config.getOneRoutes(get.getUrl());

	if (temp == NULL)
	{
		buildNewURl(config.getRoot(), get.getUrl());
		return ;
	}
	get.setRoute(temp);
	get.setIsRoute(true);
	if (!checkAllowMeth(*temp, get.getMethod()))
	{
		get.setResponse(405, "Not Allowed", config.getErrorPage(405));
		return ;
	}
	const std::string &redir = temp->getRedirection().url;
	if (!redir.empty())
		get.setResponse(301, "Moved Permanently", redir);
	else
		replaceUrl(config.getLocation(get.getUrl()), temp->getRoot(), get.getUrl());
}
