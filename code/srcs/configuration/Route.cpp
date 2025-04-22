#include <stddef.h>      // for size_t
#include <iostream>      // for basic_ostream, operator<<, char_traits, endl
#include <string>        // for basic_string, string, operator<<
#include <vector>        // for vector

#include "EMethods.hpp"  // for EMethods
#include "Route.hpp"     // for Route, SRedirection, operator<<

Route::Route
(
	const std::vector<EMethods> &acceptedMethods,
	const SRedirection &redirection,
	const std::vector<std::string> &index,
	const bool &auto_index,
	const std::string &root,
	const std::string &cgiFileExtension,
	const bool	&acceptUploads
) :
	acceptedMethods(acceptedMethods),
	redirection(redirection),
	index(index),
	autoIndex(auto_index),
	root(root),
	cgiFileExtension(cgiFileExtension),
	acceptUploads(acceptUploads)
{
}

Route::Route(Route const &src)
{
	this->acceptedMethods = src.acceptedMethods;
	this->redirection = src.redirection;
	this->index = src.index;
	this->autoIndex = src.autoIndex;
	this->root = src.root;
	this->cgiFileExtension = src.cgiFileExtension;
	this->acceptUploads = src.acceptUploads;
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

bool	Route::getAutoIndex(void) const
{
	return (autoIndex);
}

const std::string				&Route::getRoot(void) const
{
	return (root);
}

const std::string				&Route::getCgiFileExtension(void) const
{
	return (cgiFileExtension);
}

bool	Route::getAcceptUploads(void) const
{
	return (acceptUploads);
}

void							Route::setIndex(const std::vector<std::string> &v)
{
	this->index = v;
}

std::ostream & operator<<(std::ostream & o, Route const & rhs)
{
	const std::vector<std::string>	&index = rhs.getIndex();
	const std::vector<EMethods> 	&acceptedMethods = rhs.getAcceptedMethods();
	const std::string				&root = rhs.getRoot();
	const SRedirection				&redirection = rhs.getRedirection();

    o << "auto_index :" << rhs.getAutoIndex() << '\n';
	if (!root.empty())
		o << "root :" << rhs.getRoot() << '\n';
	o << "index : ";
	for (size_t i = 0; i < index.size(); i++)
	{
		o << index[i] << " ";
	}
	o << std::endl;
	o << "accepted methods :";
	for (size_t i = 0; i < acceptedMethods.size(); i++)
	{
		o << getStringRepresentation(acceptedMethods[i]);
	}
	o << '\n';
	if (!redirection.url.empty())
	{
		o << "redirection status code:" << redirection.responseStatusCode << '\n';
		o << "redirection url:" << redirection.url << '\n';
	}
	o << "accepted uploads:" << rhs.getAcceptUploads() << '\n';
    return (o);
}
