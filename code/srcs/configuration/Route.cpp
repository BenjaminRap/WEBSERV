#include "Route.hpp"
#include <iostream>

Route::Route(	std::vector<EMethods> acceptedMethods, \
			SRedirection redirection, \
			std::vector<std::string> index, \
			bool auto_index, \
			std::string root, \
			bool directoryListing, \
			std::string directoryFile, \
			std::string cgiFileExtension, \
			SUploads	uploads) : \
			acceptedMethods(acceptedMethods), \
			redirection(redirection), \
			index(index), \
			auto_index(auto_index), \
			root(root), \
			directoryListing(directoryListing), \
			directoryFile(directoryFile), \
			cgiFileExtension(cgiFileExtension), \
			uploads(uploads)
			{
			}

Route::~Route(void)
{

}

const std::vector<EMethods>		&Route::getAcceptedMethods(void) const
{
	return (acceptedMethods);
}

const SRedirection				&Route::getRedirection(void) const
{
	return (redirection);
}

const std::vector<std::string>	&Route::getIndex(void) const
{
	return (index);
}

const bool						&Route::getAutoIndex(void) const
{
	return (auto_index);
}

const std::string				&Route::getRoot(void) const
{
	return (root);
}

const bool						&Route::getDirectoryListing(void) const
{
	return (directoryListing);
}

const std::string				&Route::getDirectoryFile(void) const
{
	return (directoryFile);
}

const std::string				&Route::getCgiFileExtension(void) const
{
	return (cgiFileExtension);
}

const SUploads					&Route::getUploads(void) const
{
	return (uploads);
}

std::ostream & operator<<(std::ostream & o, Route const & rhs)
{
	const std::vector<std::string>	&index = rhs.getIndex();
	const std::vector<EMethods> 	&acceptedMethods = rhs.getAcceptedMethods();

    o << "auto_index :" << rhs.getAutoIndex() << std::endl;
	o << "root :" << rhs.getRoot() << std::endl;
	o << "index : ";
	for (size_t i = 0; i < index.size(); i++)
	{
		o << index[i] << " ";
	}
	o << std::endl;
	o << "accepted methods :";
	for (size_t i = 0; i < acceptedMethods.size(); i++)
	{
		if (acceptedMethods[i] == GET)
			o << "GET ";
		else if (acceptedMethods[i] == POST)
			o << "POST ";
		else if (acceptedMethods[i] == DELETE)
			o << "DELETE ";
	}
	o << std::endl;
	o << "redirection status code:" << rhs.getRedirection().responseStatusCode << std::endl;
	o << "redirection url:" << rhs.getRedirection().url << std::endl;
    return (o);
}
