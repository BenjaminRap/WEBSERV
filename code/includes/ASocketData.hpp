#ifndef A_SOCKET_DATA_HPP
# define A_SOCKET_DATA_HPP

# include <list>					// for std::list

# include "AFdData.hpp"				// for AFdData
# include "ServerConfiguration.hpp"	// for ServerConfiguration

/**
 * @brief Every FDs that are in the listeners of epoll has a corresponding FdData
 * instance. This class store everything that the programs needs when the fd
 * receive an events (EPOLLIN/EPOLLOUT/...).
 * This class is abstract, so it can only be used through its childs.
 */
class ASocketData : public AFdData
{
protected:
	/**
	 * @brief The iterator of this instance in the EPollHandler list. It is used
	 * to remove this instance from the list in O(1).
	 */
	std::list<ASocketData *>::iterator		_iterator;
	/**
	 * @brief True if the setIterator has been called with a valid argument.
	 */
	bool									_isIteratorSet;
	const std::vector<ServerConfiguration>	&_serverConfigurations;

	ASocketData
	(
		int fd,
		EPollHandler &ePollHandler,
		const std::vector<ServerConfiguration> &serverConfigurations,
		AFdDataChilds type
	);
private:
	ASocketData(void);
	ASocketData(const ASocketData &ref);

	ASocketData&	operator=(const ASocketData& ref);
public:
	virtual ~ASocketData(void);

	/**
	 * @brief Return the iterator pointing to this object in the SocketHandler _socketsData
	 * list.
	 * @throw If the iterator hasn't been set with the setIterator function, throw
	 * a std::logic_error.
	 * @return A const reference on the iterator pointing to this object.
	 */
	const std::list<ASocketData *>::iterator&	getIterator() const;
	/**
	 * @brief Set the _iterator of this FdData to a copy of the iterator passed
	 * as argument.
	 * If the iterator has already been set, print an error.
	 * If the FdData pointed by the iterator isn't this class, print an error.
	 * @param iterator The iterator that points to this FdData.
	 */
	void										setIterator(const std::list<ASocketData *>::iterator &iterator);
	/**
	 * @brief Remove this ASocketData from the EPollHandler list of sockets.
	 * @note Calling this function will destroy this instance.
	 */
	void										removeFromEPollHandler(void);
};

#endif // !A_SOCKET_DATA_HPP
