#ifndef RAW_RESPONSE_HPP
# define RAW_RESPONSE_HPP

# include <cstddef>

# include "FlowBuffer.hpp"
# include "Response.hpp"

/**
 * @brief This class stores all the data that will be sent to the client as a
 * response, compacted in a single string and a fd for the body. This class take
 * responsability for closing of the fd and deallocating the firstPart buffer.
 * It moves the data from the file to the socket like a stream, without allocation.
 * This class is designed to use the less syscall possible, for performances.
 */
class RawResponse
{
private:
	/**
	 * @brief The first part of the response. It is composed by the status line,
	 * the headers, the empty line and, maybe, a part of the body. It should be
	 * allocated in the heap.
	 */
	FlowBuffer	_firstPart;
	/**
	 * @brief The file descriptor of the body. If there is no body, or it has already
	 * been included in firstPart, this variable is set to -1.
	 */
	int			_bodyFd;
	/**
	 * @brief A pointer on the ResponsesHandler FlowBuffer. It allows this class
	 * to redirect the content from _bodyFd to the client socket Fd. If there is
	 * no body, this variable is set to NULL.
	 */
	FlowBuffer	*_bodyBuffer;

	RawResponse();

public:
	RawResponse(const RawResponse& ref);
	RawResponse&	operator=(const RawResponse& ref);
	RawResponse(char *firstPart, size_t firstPartLength, int bodyFd, FlowBuffer &bodyFlowBuffer);
	RawResponse(char *firstPart, size_t firstPartLength);
	RawResponse(const Response &src, FlowBuffer &bodyFlowBuffer);
	~RawResponse();

	FlowState	sendResponseToSocket(int socketFd);
};

#endif // !RAW_RESPONSE_HPP