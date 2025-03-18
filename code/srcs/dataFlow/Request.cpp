#include "Request.hpp"
#include "SizedBody.hpp"

Request::Request(void) : _body(NULL)
{
	return ;
}

void	Request::reset()
{
	this->statusLine.requestTarget.clear();
	this->_headers.clear();
	if (_body != NULL)
	{
		delete _body;
		_body = NULL;
	}
}

Body	*Request::getBody() const
{
	return (_body);
}

int		Request::parseStatusLine(const char *line, size_t lineLength)
{
	//Parsing the method
	const char	*meth = std::find(line, line + lineLength, ' ');
	if (*meth != ' ')
		return (400);
	if (!std::memcmp(line, "GET", 3))
		this->statusLine.method = GET;
	else if (!std::memcmp(line, "POST", 4))
		this->statusLine.method = POST;
	else if (!std::memcmp(line, "DELETE", 6))
		this->statusLine.method = DELETE;
	else if (!std::memcmp(line, "PUT", 3))
		this->statusLine.method = PUT;
	else
		return (501);

	//Parsing the target
	const char	*targ = std::find(meth + 1, line + lineLength, ' ');
	if (*targ != ' ')
		return (400);
	this->statusLine.requestTarget = std::string(meth + 1, targ - (meth + 1));

	//Parsing the protocol
	const char	*prot = std::find(targ + 1, line + lineLength, '\r');
	if (*prot != '\r' || *(prot + 1) != '\n')
		return (400);
	if (std::memcmp(targ + 1, "HTTP/1.1", 8))
		return (505);
	return (0);
}

int		Request::parseHeader(const char *line, size_t lineLength)
{
	const char *	pos;
	const char *	temp;

	pos = std::find(line, line + lineLength, ':');
	if (*pos == '\0' || *(pos + 1) != ' ')
		return (1);
	temp = std::find(line, line + lineLength, '\r');
	if (*temp == '\0' || *(temp + 1) != '\n')
		return (2);
	std::string key(line, pos - line);
	std::string value(pos + 2, temp - (pos + 2));
	this->_headers.insert(std::make_pair(key, value));
	return (0);
}

Request::~Request(void)
{
	if (_body != NULL)
		delete _body;
}

EMethods	Request::getMethod(void) const
{
	return (this->statusLine.method);
}

const std::string	&Request::getRequestTarget(void) const
{
	return (this->statusLine.requestTarget);
}

const std::string	*Request::getHeader(const std::string &key) const
{
	std::map<std::string, std::string>::const_iterator it = this->_headers.find(key);

	if (it != this->_headers.end())
		return (&it->second);
	return (NULL);
}

const std::map<std::string, std::string>	&Request::getHeaderMap(void) const
{
	return (this->_headers);
}


bool	stringToSizeT(const  std::string &str, size_t &outValue);

int	Request::setBodyFromHeaders(int destFd, bool isBlocking)
{
	const std::string * const	contentLengthString = getHeader("Content-Length");
	if (contentLengthString != NULL)
	{
		size_t contentLength = 0;
		if (stringToSizeT(*contentLengthString, contentLength) == false)
			return (400);
		try
		{
			_body = new SizedBody(destFd, contentLength, isBlocking);
		}
		catch (const std::exception&)
		{
			return (500);
		}
		return (0);
	}
	return (411);
}

std::ostream & operator<<(std::ostream & o, Request const & rhs)
{
	const std::map<std::string, std::string>	&header = rhs.getHeaderMap();
	std::string	method[4] = {"GET", "POST", "DELETE", "PUT"};

	std::cout << "Method :" << method[rhs.getMethod()] << std::endl;
	std::cout << "Target :" << rhs.getRequestTarget() << std::endl;
	std::cout << "Protocol :" << "HTTP/1.1" << std::endl << std::endl;

	for (std::map<std::string ,std::string>::const_iterator it = header.begin(); it != header.end(); ++it)
	{
		std::cout << it->first << ": " << it->second << std::endl;
	}
	return (o);
}
