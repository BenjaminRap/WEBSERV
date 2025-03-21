#include "SharedResource.hpp"	// for SharedResource

/*****************************Constructors / Destructors*******************************/

/*
* @brief Create an instance of the SharedResource class with no
* managed resources. Itt does nothing for now but can be attributed
* a resource with the operator= overload.
* @throw This constructor can throw if the T class default constructor
* throws. If it doesn't , this constructor doesn't.
* If it throws, there is no side effects on this class.
*/
template <typename T>
SharedResource<T>::SharedResource(void) : 
	value(), // can throw
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
* If it throws, there is no side effects on this class.
*/
template <typename T>
SharedResource<T>::SharedResource(T value, void (&free)(T value)) : 
	value(value), // can throw
	_count(new size_t), // can throw
	_free(&free)
{
	(*_count) = 1;
}

/*
* @brief Create an instance of the SharedResource class
* that manages the same value as the ref instance, increment
* the number of instances sharing the value.
* @throw This function can throw if the free function throws or if the
* operator= overload of the T class throws.
*/
template <typename T>
SharedResource<T>::SharedResource(const SharedResource<T> &ref) :
	value(ref.value),
	_count(ref._count),
	_free(ref._free)
{
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
	decrementCount();
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
	decrementCount(); // can throw
	value = ref.value; // can throw
	_free = ref._free;
	_count = ref._count;
	(*_count) += 1;
	return (*this);
}

/**************************************Methods*********************************************/

template <typename T>
void	SharedResource<T>::decrementCount(void)
{
	if (_count != NULL)
	{
		(*_count) -= 1;
		if (*_count == 0)
		{
			delete _count;
			_count = NULL;
			_free(value); // can throw
		}
	}
}

/***********************************External functions**************************************/


template <typename Pointer>
void	freePointer(Pointer *pointer)
{
	delete pointer;
}
