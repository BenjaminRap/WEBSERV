#ifndef SHARED_RESOURCE_HPP
# define SHARED_RESOURCE_HPP

# include <cstddef>

/*
* @brief This class allows a resource to be automatically free'd when there is no remaining
* instance of this class sharing it.
* It should be used with allocated memory, fds, DIR ...
* It allows for a leak proof c++.
* It is the equivalent of a shared_ptr, but for anything that needs to be closed, freed ...
* A SharedResource can be instanciated with no managed resource.
* A SharedResource can be attributed a resource, or reattributed, after its instanciation
* with the operator= overload or the setManagedResource method.
*/
template <typename T>
class SharedResource
{
public:

	/*
	* @brief Create an instance of the SharedResource class with no
	* managed resources. It does nothing for now but can be attributed
	* a resource with the operator= overload or the setManagedResource method.
	* @throw This constructor can throw if the T class default constructor
	* throws. If it doesn't , this constructor doesn't.
	* If it throws, there is no side effects on this class.
	*/
	SharedResource(void);
	/*
	* @brief Create an instance of the SharedResource class, 
	* managing the value and calling free with value as parameter
	* when no instance of the SharedResource class exists.
	* @throw This constructor can throw if the T class copy constructor
	* throws or throw a std::bad_alloc.
	* If a std::bad_alloc occurs, the value is freed.
	* If it throws, there is no side effects on this class.
	*/
	SharedResource(T value, void (&free)(T value));
	/*
	* @brief Create an instance of the SharedResource class
	* that manages the same value as the ref instance, increment
	* the number of instances sharing the value.
	* @throw This function can throw if the operator= overload
	* of the T class throws.
	*/
	SharedResource(const SharedResource &ref);
	/*
	* @brief Changes the value managed by this instance to the ref's
	* managed value.
	* If this instance already managed a value, it will stops
	* managing it.
	* @throw This function can throw if the free function throws or if the
	* operator= overload of the T class throws.
	* If this function throws, this instance will loose its management over
	* the resource it had before the call.
	*/
	SharedResource &operator=(const SharedResource &ref) ;

	/*
	* @brief Destruct the objet and decrement the count of instances
	* sharing the resource. It the count fall to 0, the resource is freed.
	* @throw This function can throw if the free function throw.
	*/
	~SharedResource(void);

	/*
	* @brief Set the managed resource of this instance to the value.
	* @note This value shouldn't be managed by another SharedResource.
	* @throw It can throw a std::bad_alloc or if the previous free function
	* fail or if the T copy constructor fail.
	* If it throws a std::bad_alloc, the value is freed
	*/
	void	setManagedResource(T value, void (&free)(T value));
	/*
	* @brief This method reduce the count and free it if it drop to 0.
	* @throw This function can throw if the _free function throws.
	*/
	void	stopManagingResource(void);
	/**
	 * @brief Return a reference on the managed resource.
	 * @throw If this instance doesn't manage a value, throw a std::logic_error
	 */
	T		&getValue(void);
	/**
	 * @brief Return a const reference on the managed resource.
	 * @throw If this instance doesn't manage a value, throw a std::logic_error
	 */
	const T	&getValue(void) const;
	/**
	 * @brief Return true if this instance manages a resource, false otherwise
	 */
	bool	isManagingValue(void) const;
private:
	T		_value;
	size_t	*_count;
	void	(*_free)(T value);

};

/*
* @brief call delete on the pointer.
* @throw Can throw if the Pointer class destructor throws.
*/
template <typename Pointer>
void	freePointer(Pointer *pointer);

# include "SharedResource.ipp"

#endif // !SHARED_RESOURCE_HPP
