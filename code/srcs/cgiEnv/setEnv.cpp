#include <iostream>
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

void	printEnv(char **env)
{
	std::cout << std::endl;
	std::cout << CYAN << "--------------		setEnv		--------------" << RESET << std::endl << std::endl;
	int i = 0;
	while (env[i])
	{
		std::cout << GREEN << env[i] << RESET << std::endl;
		i++;
	}
	std::cout << CYAN << "------------------------------------------------------" << RESET << std::endl << std::endl;

}

char *duplicateString(const std::string &str)
{
	char *dup = new char[str.length() + 1];
	if (dup == NULL)
		return (NULL);
	std::strcpy(dup, str.c_str());
	return (dup);
}

bool	addToEnv(char ***env, const std::string &title, const std::string *value)
{
	std::string newElement;
	std::string temp;
	if (value == NULL)
		temp = "";
	else
		temp = *value;
	newElement = title + temp;
	int i = 0;
	while ((*env)[i] != NULL)
		i++;
	(*env)[i] = duplicateString(newElement);
	if ((*env)[i] == NULL)
		return (false);
	(*env)[i + 1] = NULL;
	return (true);
}

const std::string *checkHeader(const std::string &name, Request *request)
{
	const std::string *temp = request->getHeader(name);
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

char	**setEnv(Request *request, size_t lenght)
{
	char	**env;
	std::string temp;

	env = new char *[20];
	if (env == NULL)
		return (NULL);
	env[0] = NULL;
	addToEnv(&env, "SERVER_SOFTWARE=" SERVER_SOFTWARE, NULL);
	addToEnv(&env, "SERVER_NAME=", checkHeader("Host", request));
	addToEnv(&env, "GATEWAY_INTERFACE=" GATEWAY_INTERFACE, NULL);
	addToEnv(&env, "SERVER_PROTOCOL=" PROTOCOL, NULL);
	addToEnv(&env, "REQUEST_METHOD=" + getStringFromMethod(request->getMethod()), NULL);
	addToEnv(&env, "HTTP_ACCEPT=", checkHeader("Accept", request));
	addToEnv(&env, "HTTP_ACCEPT_LANGUAGE=", checkHeader("Accept-Language", request));
	addToEnv(&env, "HTTP_USER_AGENT=", checkHeader("User-Agent", request));
	addToEnv(&env, "HTTP_COOKIE=", checkHeader("Cookie", request));
	addToEnv(&env, "CONTENT_TYPE=", checkHeader("content-type", request));
	addToEnv(&env, "CONTENT_LENGTH=" + sizeTToString(lenght), NULL);
	addToEnv(&env, "REFERER=", checkHeader("Referer", request));
	printEnv(env);
	return (env);
}