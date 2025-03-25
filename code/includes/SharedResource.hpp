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
* with the operator= overload.
*/
template <typename T>
class SharedResource
{
public:

	SharedResource(void);
	SharedResource(T value, void (&free)(T value));
	SharedResource(const SharedResource &ref);
	SharedResource &operator=(const SharedResource &ref) ;

	~SharedResource(void);

	void	setManagedResource(T value, void (&free)(T value));
	T		&getValue(void) const;
	bool	isManagingValue(void) const;
	void	stopManagingResource(void);
private:
	T		_value;
	size_t	*_count;
	void	(*_free)(T value);

};

template <typename Pointer>
void	freePointer(Pointer *pointer);

# include "SharedResource.ipp"

#endif // !SHARED_RESOURCE_HPP
