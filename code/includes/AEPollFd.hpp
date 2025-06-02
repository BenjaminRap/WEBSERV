#ifndef A_EPOLL_FD_HPP
# define A_EPOLL_FD_HPP

# include <list>					// for std::list

# include "AFdData.hpp"				// for AFdData

/**
 * @brief Every FDs that are in the listeners of epoll has a corresponding AEPollFd
 * instance. This class store everything that the programs needs when the fd
 * receive an events (EPOLLIN/EPOLLOUT/...).
 * This class is abstract, so it can only be used through its childs.
 */
class AEPollFd : public AFdData
{
protected:
	/**
	 * @brief The iterator of this instance in the EPollHandler list. It is used
	 * to remove this instance from the list in O(1).
	 */
	std::list<AEPollFd *>::iterator		_iterator;
	/**
	 * @brief True if the setIterator has been called with a valid argument.
	 */
	bool								_isIteratorSet;
	/**
	 * @brief The class managing all fds in epoll, including this one.
	 */
	EPollHandler&						_ePollHandler;

	/**
	 * @brief This constructor add the fd to the epoll interest list
	 * with the events passed as arguments.
	 * @throw It throws a std::logic_error if the events are set to 0.
	 */
	AEPollFd
	(
		int fd,
		EPollHandler &ePollHandler,
		AFdDataChilds type,
		uint32_t events
	);
	/**
	 * @brief This constructor does not set the fd. SetFd should be called
	 * later to set it and add it to epoll.
	 */
	AEPollFd
	(
		EPollHandler &ePollHandler,
		AFdDataChilds type
	);

	/**
	 * @brief Set _isActive to false, and, if the iterator has been set,
	 * add it in the EPollHandler _ePollFdToRemove list.
	 * It will be removed after the callbacks of the current loop.
	 */
	void										removeFromEPollHandler(void);
private:

	/**
	 * @brief Add the O_NONBLOCK flag to the fd, and add it to the epoll
	 * interest list.
	 * @throw This function throws a std::logic_error if events is set to 0.
	 *
	 * @param events The events on which epoll will notify us.
	 */
	void										initFd(uint32_t events);

	AEPollFd(void);
	AEPollFd(const AEPollFd &ref);

	AEPollFd&	operator=(const AEPollFd& ref);
public:
	virtual ~AEPollFd(void);

	/**
	 * @brief call the instance removeFromEPollHandler method.
	 * The AFdData passed as arugment ins static_casted into a AEPollFd
	 */
	static void									removeFromEPollHandler(AFdData* fdData);
	/**
	 * @brief Return the iterator pointing to this object in the EPollHandler _ePollFds
	 * list.
	 * @throw If the iterator hasn't been set with the setIterator function, throw
	 * a std::logic_error.
	 * @return A const reference on the iterator pointing to this object.
	 */
	const std::list<AEPollFd *>::iterator&		getIterator() const;
	/**
	 * @brief Set the _iterator of this FdData to a copy of the iterator passed
	 * as argument.
	 * If the iterator has already been set, print an error.
	 * If the FdData pointed by the iterator isn't this class, print an error.
	 * @param iterator The iterator that points to this FdData.
	 *
	 */
	void										setIterator(const std::list<AEPollFd *>::iterator &iterator);
	/**
	 * @brief The the fd and initialize it.
	 * @ref initFd.
	 * @throw :
	 * 	- a std::logic_error if the fd is already set
	 * 	- a std::invalid_argument if the newFd is negative
	 * 	- std::runtime_error if we can't add FD_CLOEXEC to the fd
	 * 	- std::logic_error if the events are set to 0.
	 *
	 */
	void										setFd(int fd, uint32_t events);
};

#endif // !A_EPOLL_FD_HPP
