// IWYU pragma: private, include "SharedResource.hpp"

#include <stdexcept>			// for logic_error

#include "SharedResource.hpp"	// for SharedResource

/*****************************Constructors / Destructors*******************************/

template <typename T>
SharedResource<T>::SharedResource(void) : 
	_value(), // can throw
	_count(NULL),
	_free(NULL)
{
}

template <typename T>
SharedResource<T>::SharedResource(T value, void (&free)(T value)) : 
	_value(value), // can throw
	_count(NULL), // can throw
	_free(&free)
{
	try
	{
		_count = new size_t;
		(*_count) = 1;
	}
	catch (const std::exception& exception)
	{
		free(value);
		throw exception;
	}
}

template <typename T>
SharedResource<T>::SharedResource(const SharedResource<T> &ref) :
	_value(ref._value),
	_count(ref._count),
	_free(ref._free)
{
	if (_count != NULL)
		(*_count) += 1;
}

template <typename T>
SharedResource<T>::~SharedResource<T>(void)
{
	stopManagingResource();
}

/****************************Operator Overload*******************************************/

template <typename T>
SharedResource<T>&	SharedResource<T>::operator=(const SharedResource<T> &ref)
{
	stopManagingResource(); // can throw
	_value = ref._value; // can throw
	_free = ref._free;
	_count = ref._count;
	if (_count != NULL)
		(*_count) += 1;
	return (*this);
}

/**************************************Methods*********************************************/

template <typename T>
void	SharedResource<T>::setManagedResource(T value, void (&free)(T value))
{
	try
	{
		stopManagingResource(); // can throw
		_value = value; // can throw
		_count = new size_t; // can throw
		_free = free;
		(*_count) = 1;
	}
	catch (const std::exception& exception)
	{
		free(value);
		throw exception;
	}
}

template <typename T>
void	SharedResource<T>::stopManagingResource(void)
{
	if (_count != NULL)
	{
		(*_count) -= 1;
		if (*_count == 0)
		{
			delete _count;
			_count = NULL;
			_free(_value); // can throw
		}
		else
			_count = NULL;
	}
}

template <typename T>
T&	SharedResource<T>::getValue(void)
{
	if (_count == NULL)
		throw std::logic_error("getValue called with a SharedResource managing nothing!");
	return (_value);
}

template <typename T>
const T&	SharedResource<T>::getValue(void) const
{
	if (_count == NULL)
		throw std::logic_error("getValue called with a SharedResource managing nothing!");
	return (_value);
}

template <typename T>
bool	SharedResource<T>::isManagingValue(void) const
{
	return (_count != NULL);
}

/***********************************External functions**************************************/

template <typename Pointer>
void	freePointer(Pointer *pointer)
{
	delete pointer;
}
