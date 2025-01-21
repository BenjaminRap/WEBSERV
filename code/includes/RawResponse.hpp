#ifndef RAW_RESPONSE_HPP
# define RAW_RESPONSE_HPP

# include <string>
# include <sys/types.h>
# include <cstddef>

# include "FlowBuffer.hpp"

/**
 * @brief This class stores all the data that will be sent to the client as a
 * response, compacted in a single string and a fd for the body. This class take
 * responsability for the closing of the fd.
 * It moves the data from the file to the socket like a stream, without allocation.
 * This class is designed to be minimalistic.
 */
class RawResponse
{
private:
	/**
	 * @brief The buffer for the first part of the responses : the status line, 
	 * the headers and, optionnaly, the body or the first part of the body.
	 */
	FlowBuffer	_firstPartBuffer;
	/**
	 * @brief The file descriptor of the body.If there is no body, or it has already
	 * been included in firstPart, this variable is set to -1.
	 */
	int			_bodyFd;
	/**
	 * @brief A reference on the ResponsesHandler FlowBuffer. It allows this class
	 * to redirect the content from _bodyFd to the client socket Fd.
	 */
	FlowBuffer	&_bodyBuffer;

	RawResponse();

	RawResponse&	operator=(const RawResponse& ref);
public:
	RawResponse(const RawResponse& ref);
	RawResponse(char *buffer, std::size_t bufferCapacity, int bodyFd, FlowBuffer &bodyBuffer);
	~RawResponse();

};

#endif // !RAW_RESPONSE_HPP