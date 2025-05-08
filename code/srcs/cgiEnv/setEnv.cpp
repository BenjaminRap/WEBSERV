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

	std::strcpy(dup, str.c_str());
	return (dup);
}

bool	addToEnv(char *(&env)[20], const std::string &title, const std::string *value)
{
	if (value == NULL)
		return (true);

	int i = 0;
	while (env[i] != NULL && i < 18)
		i++;
	if (i == 19)
		throw std::logic_error("Too much environment variable !");
	env[i] = duplicateString(title + *value);
	env[i + 1] = NULL;
	return (true);
}

bool	addToEnv(char *(&env)[20], const std::string &title)
{
	int i = 0;
	while (env[i] != NULL && i < 18)
		i++;
	if (i == 19)
		throw std::logic_error("Too much environment variable !");
	env[i] = duplicateString(title);
	env[i + 1] = NULL;
	return (true);
}

std::string findScriptName(const std::string &target, size_t &pos, const std::string& extension)
{
	const size_t end = target.find(extension);

	if (end == std::string::npos)
		throw std::logic_error("setEnv called with an invalid file extension");
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

	return (target.substr(pos));
}

char	**setEnv(Request &request, char *(&env)[20], const std::string& extension)
{
	const std::string&	target = request.getRequestTarget();
	const Headers&		headers = request.getHeaders();

	size_t pos = 0;

	env[0] = NULL;
	addToEnv(env, "SERVER_SOFTWARE=" SERVER_SOFTWARE);
	addToEnv(env, "SERVER_NAME=", headers.getHeader("Host"));
	addToEnv(env, "GATEWAY_INTERFACE=" GATEWAY_INTERFACE);
	addToEnv(env, "SERVER_PROTOCOL=" PROTOCOL);
	addToEnv(env, "REQUEST_METHOD=" + getStringRepresentation(request.getMethod()));
	addToEnv(env, "HTTP_ACCEPT=", headers.getHeader("Accept"));
	addToEnv(env, "HTTP_ACCEPT_LANGUAGE=", headers.getHeader("Accept-Language"));
	addToEnv(env, "HTTP_USER_AGENT=", headers.getHeader("User-Agent"));
	addToEnv(env, "HTTP_COOKIE=", headers.getHeader("Cookie"));
	addToEnv(env, "CONTENT_TYPE=", headers.getHeader("content-type"));
	addToEnv(env, "CONTENT_LENGTH=", headers.getHeader("content-length"));
	addToEnv(env, "REFERER=", headers.getHeader("Referer"));
	addToEnv(env, "SCRIPT_NAME=" + findScriptName(target, pos, extension));
	addToEnv(env, "PATH_INFO=" + findPathInfo(target, pos));
	addToEnv(env, "QUERY_STRING=" + findQueryString(target, pos));
	return (env);
}
