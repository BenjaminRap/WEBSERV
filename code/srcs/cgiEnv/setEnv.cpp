#include <string.h>
#include "Request.hpp"
#include "protocol.hpp"

#define SERVER_SOFTWARE "webserv/1.0"
#define GATEWAY_INTERFACE "CGI/1.1"

#define RESET   "\033[0m"
#define RED     "\033[1;31m"
#define GREEN   "\033[1;32m"
#define YELLOW  "\033[1;33m"
#define BLUE    "\033[1;34m"
#define MAGENTA "\033[1;35m"
#define CYAN    "\033[1;36m"
#define WHITE   "\033[1;37m"


std::string	sizeTToString(size_t value);

char *duplicateString(const std::string &str)
{
	char *dup = new char[str.length() + 1];

	if (dup == NULL)
		return (NULL);
	std::strcpy(dup, str.c_str());
	return (dup);
}

bool	addToEnv(char *(&env)[20], const std::string &title, const std::string *value)
{
	std::string newElement;
	std::string temp;

	if (value == NULL)
		temp = "";
	else
		temp = *value;
	newElement = title + temp;
	int i = 0;
	while (env[i] != NULL)
		i++;
	env[i] = duplicateString(newElement);
	if (env[i] == NULL)
		return (false);
	env[i + 1] = NULL;
	return (true);
}

const std::string *checkHeader(const std::string &name, const Headers& headers)
{
	const std::string *temp = headers.getHeader(name);

	if (temp == NULL || temp->empty())
		return (NULL);
	else
		return (temp);
}

std::string getStringFromMethod(EMethods method)
{
	switch (method)
	{
		case GET:
			return ("GET");
		case POST:
			return ("POST");
		case PUT:
			return ("PUT");
		case DELETE:
			return ("DELETE");
		default:
			return ("");
	}
}

std::string findScriptName(const std::string &target, size_t &pos)
{
	size_t end = target.find(".cgi");
	if (end == std::string::npos)
		return ("");
	std::string result = target.substr(0, end + 4);
	pos = end + 4;
	return (result);
}

std::string findPathInfo(const std::string &target, size_t &pos)
{
	if (target[pos] != '/')
		return ("");
	size_t end = target.find("?", pos + 1);
	std::string result;
	if (end == std::string::npos)
	{
		result = target.substr(pos);
		return (result);
	}
	else
	{
		result = target.substr(pos, end - pos);
		pos = end;
		return (result);
	}
}

std::string findQueryString(const std::string &target, size_t &pos)
{
	if (target[pos] != '?')
		return ("");

	std::string result;
	size_t end = target.find("#", pos + 1);
	if (end == std::string::npos)
	{
		result = target.substr(pos);
		return (result);
	}
	else
	{
		result = target.substr(pos, end - pos);
		pos = end;
		return (result);
	}
}

char	**setEnv(Request &request, size_t length, char *(&env)[20])
{
	const std::string&	target = request.getRequestTarget();
	const Headers&		headers = request.getHeaders();

	size_t pos = 0;

	env[0] = NULL;
	addToEnv(env, "SERVER_SOFTWARE=" SERVER_SOFTWARE, NULL);
	addToEnv(env, "SERVER_NAME=", checkHeader("Host", headers));
	addToEnv(env, "GATEWAY_INTERFACE=" GATEWAY_INTERFACE, NULL);
	addToEnv(env, "SERVER_PROTOCOL=" PROTOCOL, NULL);
	addToEnv(env, "REQUEST_METHOD=" + getStringFromMethod(request.getMethod()), NULL);
	addToEnv(env, "HTTP_ACCEPT=", checkHeader("Accept", headers));
	addToEnv(env, "HTTP_ACCEPT_LANGUAGE=", checkHeader("Accept-Language", headers));
	addToEnv(env, "HTTP_USER_AGENT=", checkHeader("User-Agent", headers));
	addToEnv(env, "HTTP_COOKIE=", checkHeader("Cookie", headers));
	addToEnv(env, "CONTENT_TYPE=", checkHeader("content-type", headers));
	addToEnv(env, "CONTENT_LENGTH=" + sizeTToString(length), NULL);
	addToEnv(env, "REFERER=", checkHeader("Referer", headers));
	addToEnv(env, "SCRIPT_NAME=" + findScriptName(target, pos), NULL);
	addToEnv(env, "PATH_INFO=" + findPathInfo(target, pos), NULL);
	addToEnv(env, "QUERY_STRING=" + findQueryString(target, pos), NULL);
	return (env);
}
