#include <cstring>       // for size_t, memset, strcpy, NULL
#include <exception>     // for exception
#include <stdexcept>     // for logic_error
#include <string>        // for basic_string, string, operator+

#include "ARequestType.hpp"
#include "EMethods.hpp"  // for getStringRepresentation, EMethods
#include "Headers.hpp"   // for Headers
#include "Request.hpp"   // for Request
#include "protocol.hpp"  // for PROTOCOL
#include "RequestContext.hpp"  // for RequestContext
#include "Host.hpp"  // for Host

std::string	sizeTToString(size_t value);
std::string uint16toString(u_int16_t nb);
std::string ipV4toString(const struct sockaddr_in &addr);
std::string ipV6toString(const struct in6_addr &ip);

char *duplicateString(const std::string &str)
{
	char *dup = new char[str.length() + 1];

	std::strcpy(dup, str.c_str());
	return (dup);
}

bool	addToEnv(char *(&env)[20], const std::string &title)
{
	int i = 0;
	while (env[i] != NULL && i < 19)
		i++;
	if (i == 19)
		throw std::logic_error("Too much environment variable !");
	env[i] = duplicateString(title);
	return (true);
}

bool	addToEnv(char *(&env)[20], const std::string &title, const std::string *value)
{
	if (value == NULL)
		return (true);

	return (addToEnv(env, title + *value));
}


std::string findScriptName(const std::string& url, const std::string& pathInfo)
{
	const size_t	pos = url.find(pathInfo);

	if (pos == std::string::npos)
		return (url);
	return (url.substr(0, pos));
}

void	deleteArray(const char** array);

bool	setEnv(char *(&env)[20], const ARequestType& req, RequestContext& requestContext)
{
	std::memset(env, 0, sizeof(env));
	try
	{
		const Headers&		headers = requestContext.request.getHeaders();
		const EMethods		method = requestContext.request.getMethod();
		const std::string&	pathInfo = req.getPathInfo();

		addToEnv(env, "SERVER_SOFTWARE=" SERVER_SOFTWARE);
		addToEnv(env, "SERVER_NAME=", headers.getUniqueHeader("host"));
		addToEnv(env, "GATEWAY_INTERFACE=" GATEWAY_INTERFACE);
		addToEnv(env, "SERVER_PROTOCOL=" PROTOCOL);
		addToEnv(env, "REQUEST_METHOD=" + getStringRepresentation(method));
		addToEnv(env, "HTTP_ACCEPT=", headers.getUniqueHeader("accept"));
		addToEnv(env, "HTTP_ACCEPT_LANGUAGE=", headers.getUniqueHeader("accept-language"));
		addToEnv(env, "HTTP_USER_AGENT=", headers.getUniqueHeader("user-agent"));
		addToEnv(env, "HTTP_COOKIE=", headers.getUniqueHeader("cookie"));
		addToEnv(env, "CONTENT_TYPE=", headers.getUniqueHeader("content-type"));
		addToEnv(env, "CONTENT_LENGTH=", headers.getUniqueHeader("content-length"));
		addToEnv(env, "REFERER=", headers.getUniqueHeader("referer"));
		addToEnv(env, "SCRIPT_NAME=" + findScriptName(req.getUrl(), pathInfo));
		if (pathInfo != "")
			addToEnv(env, "PATH_INFO=" + pathInfo);
		addToEnv(env, "QUERY_STRING=" + req.getQueryString());
		addToEnv(env, "PATH_TRANSLATED=" + req.getPath());
		sa_family_t family = requestContext.host.getFamily();
		if (family == AF_INET) {
			addToEnv(env, "SERVER_PORT" + uint16toString(requestContext.host.getipv4Addr().sin_port));
			addToEnv(env, "REMOTE_ADDR=" + ipV4toString(requestContext.clientAddr.ipv4));
		}
		else if (family == AF_INET6) {
			addToEnv(env, "SERVER_PORT" + uint16toString(requestContext.host.getipv6Addr().sin6_port));
			addToEnv(env, "REMOTE_ADDR=" + ipV6toString(requestContext.clientAddr.ipv6.sin6_addr));
		}
		return (true);
	}
	catch (const std::exception& exception)
	{
		deleteArray((const char**)env);
		return (false);
	}
}
