#ifndef ROUTE_HPP
# define ROUTE_HPP

# include <vector>
# include <string>

# include "Methods.hpp"

struct SRedirection
{
	ushort		responseStatusCode;
	std::string	url;
};

struct SUploads
{
	bool		acceptUploads;
	std::string	path;
};

class Route
{
private:
	std::vector<EMethods>	acceptedMethods;
	SRedirection			redirection;
	std::string				root;
	bool					directoryListing;
	std::string				directoryFile;
	std::string				cgiFileExtension;
	SUploads				uploads;
};

#endif // !ROUTE_HPP

