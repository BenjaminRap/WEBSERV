#include "Route.hpp"
#include <iostream>

Route::Route(	std::vector<EMethods> acceptedMethods, \
			SRedirection redirection, \
			std::vector<std::string> index, \
			bool auto_index, \
			std::string root, \
			std::string cgiFileExtension, \
			bool acceptUploads) : \
			acceptedMethods(acceptedMethods), \
			redirection(redirection), \
			index(index), \
			auto_index(auto_index), \
			root(root), \
			cgiFileExtension(cgiFileExtension), \
			acceptUploads(acceptUploads)
{
	return;
}

Route::Route(Route const &src)
{
    *this = src;
    return;
}

Route    &Route::operator=(Route const &src)
{
    if (this != &src)
    {
		this->acceptedMethods = src.acceptedMethods;
		this->redirection = src.redirection;
		this->index = src.index;
		this->auto_index = src.auto_index;
		this->root = src.root;
		this->cgiFileExtension = src.cgiFileExtension;
		this->acceptUploads = src.acceptUploads;
    }
    return (*this);
}

Route::~Route(void)
{
	return;
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


const std::string				&Route::getCgiFileExtension(void) const
{
	return (cgiFileExtension);
}

const bool					&Route::getAcceptUploads(void) const
{
	return (acceptUploads);
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
	o << "accepted uploads:" << rhs.getAcceptUploads() << std::endl;
    return (o);
}
