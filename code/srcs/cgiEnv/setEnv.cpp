#include <netinet/in.h>        // for sockaddr_in6
#include <sys/socket.h>        // for AF_INET, AF_UNIX
#include <sys/un.h>            // for sa_family_t
#include <cstring>             // for memset, strcpy, NULL, size_t
#include <sstream>             // for basic_ostream, basic_ostringstream
#include <stdexcept>           // for logic_error
#include <string>              // for basic_string, allocator, char_traits

#include "ARequestType.hpp"    // for ARequestType
#include "EMethods.hpp"        // for getStringRepresentation, EMethods
#include "Headers.hpp"         // for Headers
#include "Host.hpp"            // for Host, sockaddr_in_u
#include "Request.hpp"         // for Request
#include "RequestContext.hpp"  // for RequestContext
#include "protocol.hpp"        // for GATEWAY_INTERFACE, PROTOCOL, SERVER_SO...

std::string ipV4toString(const struct sockaddr_in &addr);
std::string ipV6toString(const struct in6_addr &ip);

char *duplicateString(const std::string &str)
{
	char *dup = new char[str.length() + 1];

	std::strcpy(dup, str.c_str());
	return (dup);
}

bool	addToEnv(const char *(&env)[23], const std::string &title)
{
	int i = 0;
	while (env[i] != NULL && i < 22)
		i++;
	if (i == 19)
		throw std::logic_error("Too much environment variable !");
	env[i] = duplicateString(title);
	return (true);
}

bool	addContentLengthToEnv(const char *(&env)[23], size_t contentLength)
{
	std::ostringstream	oss;

	oss << "CONTENT_LENGTH=" << contentLength;
	return (addToEnv(env, oss.str()));
}

bool	addToEnv(const char *(&env)[23], const std::string &title, const std::string *value)
{
	if (value == NULL)
		return (true);

	return (addToEnv(env, title + *value));
}

std::string findScriptName(const std::string& url, const std::string& pathInfo)
{
	if (pathInfo == "")
		return (url);
	const size_t	pos = url.find(pathInfo);

	if (pos == std::string::npos)
		return (url);
	return (url.substr(0, pos));
}

void	deleteArray(const char** array);

void	ARequestType::setEnv(const char *(&env)[23], RequestContext& requestContext)
{
	std::memset(env, 0, sizeof(env));
	const Headers&		headers = requestContext._request.getHeaders();
	const EMethods		method = requestContext._request.getMethod();

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
	if (_pathInfo != "")
	{
		addToEnv(env, "SCRIPT_NAME=" + findScriptName(_url, _pathInfo));
		addToEnv(env, "PATH_INFO=" + _pathInfo);
	}
	else
		addToEnv(env, "PATH_INFO=/youpi.bla");

	if (_queryString != "")
		addToEnv(env, "QUERY_STRING=" + _queryString);
	addToEnv(env, "PATH_TRANSLATED=" + _path);
	sa_family_t family = requestContext._host.getFamily();
	if (family == AF_UNIX)
		return ;
	addToEnv(env, "SERVER_PORT=" + requestContext._host.getPort());
	if (family == AF_INET)
		addToEnv(env, "REMOTE_ADDR=" + ipV4toString(requestContext._clientAddr.ipv4));
	else
		addToEnv(env, "REMOTE_ADDR=" + ipV6toString(requestContext._clientAddr.ipv6.sin6_addr));
}
