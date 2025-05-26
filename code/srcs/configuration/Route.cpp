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
	const std::string &cgiInterpreter
) :
	_acceptedMethods(acceptedMethods),
	_redirection(redirection),
	_index(index),
	_autoIndex(auto_index),
	_root(root),
	_cgiFileExtension(cgiFileExtension),
	_cgiInterpreter(cgiInterpreter)
{
}

Route::Route(Route const &src) :
	_acceptedMethods(src._acceptedMethods),
	_redirection(src._redirection),
	_index(src._index),
	_autoIndex(src._autoIndex),
	_root(src._root),
	_cgiFileExtension(src._cgiFileExtension),
	_cgiInterpreter(src._cgiInterpreter)
{
}

Route::~Route(void)
{
	return;
}

const std::vector<EMethods>		&Route::getAcceptedMethods(void) const
{
	return (_acceptedMethods);
}

const SRedirection				&Route::getRedirection(void) const
{
	return (_redirection);
}

const std::vector<std::string>	&Route::getIndex(void) const
{
	return (_index);
}

bool	Route::getAutoIndex(void) const
{
	return (_autoIndex);
}

const std::string				&Route::getRoot(void) const
{
	return (_root);
}

const std::string				&Route::getCgiFileExtension(void) const
{
	return (_cgiFileExtension);
}

const std::string&				Route::getCgiInterpreter(void) const
{
	return (_cgiInterpreter);
}

void							Route::setIndex(const std::vector<std::string> &v)
{
	this->_index = v;
}

std::ostream & operator<<(std::ostream & o, Route const & rhs)
{
	const std::vector<std::string>	&index = rhs.getIndex();
	const std::vector<EMethods> 	&acceptedMethods = rhs.getAcceptedMethods();
	const std::string				&root = rhs.getRoot();
	const SRedirection				&redirection = rhs.getRedirection();
	const std::string				&cgiFileExtension = rhs.getRoot();

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
	if (!cgiFileExtension.empty())
		o << "cgiFileExtension:" << cgiFileExtension << std::endl;
	return (o);
}
