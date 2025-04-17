#include <cerrno>
#include <cstdlib>
#include <string>
#include <climits>
#include <sstream>
#include <netinet/in.h>


bool	stringToSizeT(const  std::string &str, size_t &outValue)
{
	char	*endPtr;
	outValue = std::strtoul(str.c_str(), &endPtr, 10);
	if (endPtr == str.c_str() || *endPtr != '\0')
		return (false);
	if (outValue == ULONG_MAX && errno == ERANGE)
		return (false);
	return (true);
}

std::string	sizeTToString(size_t value)
{
	std::ostringstream	strStream;

	strStream << value;
	return (strStream.str());
}

std::string ipV6toString(const struct in6_addr &ip)
{
	std::ostringstream ipStream;
	int i = 0;
	int j = 0;
	bool onStart = true;
	while (i < 16)
	{
		u_int16_t group = (ip.__in6_u.__u6_addr8[i] << 8) | ip.__in6_u.__u6_addr8[i + 1];
		if (group == 0)
		{
			j++;
			if (j == 8)
				ipStream << "::";
		}
		else
		{
			if (j != 0)
			{
				if (j > 1 && onStart)
					ipStream << "::";
				else if (j > 1 && !onStart)
					ipStream << ":";
				else
					ipStream << "0:";
				j = 0;
			}
			ipStream << std::hex << group;
			if (i < 14)
				ipStream << ":";
			onStart = false;
		}
		i = i + 2;
	}
	return (ipStream.str());
}

std::string ipV4toString(const struct sockaddr_in &addr)
{
	std::ostringstream ipStream;
	ipStream << (addr.sin_addr.s_addr & 0xFF) << "."
			 << ((addr.sin_addr.s_addr >> 8) & 0xFF) << "."
			 << ((addr.sin_addr.s_addr >> 16) & 0xFF) << "."
			 << ((addr.sin_addr.s_addr >> 24) & 0xFF);

	return ipStream.str();
}

std::string uint16toString(u_int16_t nb)
{
	std::ostringstream oss;
	oss << nb;
	return oss.str();
}
