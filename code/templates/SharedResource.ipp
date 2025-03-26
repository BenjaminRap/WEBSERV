#include <stdexcept>			// for logic_error

#include "SharedResource.hpp"	// for SharedResource

/*****************************Constructors / Destructors*******************************/

/*
* @brief Create an instance of the SharedResource class with no
* managed resources. It does nothing for now but can be attributed
* a resource with the operator= overload.
* @throw This constructor can throw if the T class default constructor
* throws. If it doesn't , this constructor doesn't.
* If it throws, there is no side effects on this class.
*/
template <typename T>
SharedResource<T>::SharedResource(void) : 
	_value(), // can throw
	_count(NULL),
	_free(NULL)
{
}

/*
* @brief Create an instance of the SharedResource class, 
* managing the value and calling free with value as parameter
* when no instance of the SharedResource class exists.
* @throw This constructor can throw if the T class copy constructor
* throws or throw a std::bad_alloc.
* If a std::bad_alloc occurs, the value is freed.
* If it throws, there is no side effects on this class.
*/
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

/*
* @brief Create an instance of the SharedResource class
* that manages the same value as the ref instance, increment
* the number of instances sharing the value.
* @throw This function can throw if the operator= overload
* of the T class throws.
*/
template <typename T>
SharedResource<T>::SharedResource(const SharedResource<T> &ref) :
	_value(ref._value),
	_count(ref._count),
	_free(ref._free)
{
	if (_count != NULL)
		(*_count) += 1;
}

/*
* @brief Destruct the objet and decrement the count of instances
* sharing the resource. It the count fall to 0, the resource is freed.
* @throw This function can throw if the free function throw.
*/
template <typename T>
SharedResource<T>::~SharedResource<T>(void)
{
	stopManagingResource();
}

/****************************Operator Overload*******************************************/

/*
* @brief Create an instance of the SharedResource class
* that manages the same value as the ref instance, increment
* the number of instances sharing the value.
* If this instance already managed a value, it will stops
* managing it.
* @throw This function can throw if the free function throws or if the
* operator= overload of the T class throws.
* If this function throws, this instance will loose its management over
* the resource it had before the call.
*/
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

/*
* @brief Set the managed resource of this instance to the value.
* @note This value shouldn't be managed by another SharedResource.
* @throw It can throw a std::bad_alloc or if the previous free function
* fail or if the T copy constructor fail.
* If it throws a std::bad_alloc, the value is freed
*/
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

/*
* @brief This method reduce the count and free it if it drop to 0.
* @throw This function can throw if the _free function throws.
*/
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

/**
 * @brief Return a reference on the managed resource.
 * @throw If this instance doesn't manage a value, throw a std::logic_error
 */
template <typename T>
T&	SharedResource<T>::getValue(void)
{
	if (_count == NULL)
		throw std::logic_error("getValue called with a SharedResource managing nothing!");
	return (_value);
}

/**
 * @brief Return a reference on the managed resource.
 * @throw If this instance doesn't manage a value, throw a std::logic_error
 */
template <typename T>
const T&	SharedResource<T>::getValue(void) const
{
	if (_count == NULL)
		throw std::logic_error("getValue called with a SharedResource managing nothing!");
	return (_value);
}

/**
 * @brief Return true if this instance manages a resource, false otherwise
 */
template <typename T>
bool	SharedResource<T>::isManagingValue(void) const
{
	return (_count != NULL);
}

/***********************************External functions**************************************/

/*
* @brief call delete on the pointer.
* @throw Can throw if the Pointer class destructor throws.
*/
template <typename Pointer>
void	freePointer(Pointer *pointer)
{
	delete pointer;
}
