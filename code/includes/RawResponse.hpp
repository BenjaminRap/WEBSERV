#ifndef RAW_RESPONSE_HPP
# define RAW_RESPONSE_HPP

# include <string>				// for std::string

# include "ABody.hpp"			// for ABdoy
# include "FlowBuffer.hpp"		// for FlowBuffer
# include "SharedResource.hpp"	// for SharedResource

# define LINE_END "\r\n"
# define LINE_END_LENGTH 2;

class Response;

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
	std::string					_firstPart;
	FlowBuffer					_firstPartBuffer;
	bool						_isBlocking;
	SharedResource<int>			_srcBodyFd;
	SharedResource<ABody *>		_body;
	/**
	 * @brief A pointer on the ResponsesHandler FlowBuffer. It allows this class
	 * to redirect the content from _bodyFd to the client socket Fd. If there is
	 * no body, this variable is set to NULL.
	 */
	FlowBuffer&	_bodyBuffer;

	RawResponse();
	RawResponse(const RawResponse& ref);

	RawResponse&	operator=(const RawResponse& ref);
public:
	RawResponse(Response &response, FlowBuffer &bodyBuffer);
	~RawResponse();

	FlowState	sendResponseToSocket(int socketFd);
};

#endif // !RAW_RESPONSE_HPP
