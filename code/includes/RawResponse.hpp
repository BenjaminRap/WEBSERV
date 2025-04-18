#ifndef RAW_RESPONSE_HPP
# define RAW_RESPONSE_HPP

# include <string>				// for std::string

# include "ABody.hpp"			// for ABdoy
# include "AFdData.hpp"			// for AFdData
# include "FlowBuffer.hpp"		// for FlowBuffer
# include "SharedResource.hpp"	// for SharedResource

class Response;

/**
 * @brief This class stores all the data that will be sent to the client as a
 * response, compacted in a single string and a fd for the body.
 * It moves the data from the file to the socket like a stream, without allocation.
 * This class is designed to use the less syscall possible, for performances.
 */
class RawResponse
{
private:
	/**
	 * @brief The first part of the rawResponse is a string
	 * containing : the status line, the headers, the empty line,
	 * maybe an error page and maybe the auto index page.
	 */
	std::string					_firstPart;
	/**
	 * @brief The FlowBuffer responsible for writing the first part.
	 */
	FlowBuffer					_firstPartBuffer;
	/**
	 * @brief A SharedResource on the body source fd.
	 * It isn't the fd stored in the _body, but the fd
	 * whose content will be written into the buffer.
	 * The body will then write the buffer into the client socket.
	 */
	SharedResource<AFdData*>	_fdData;
	/**
	 * @brief The body of the response, it could be a sized body, a
	 * chunked body ...
	 * @note It could also be NULL.
	 */
	SharedResource<ABody *>		_body;
	/**
	 * @brief A reference on the ResponsesHandler FlowBuffer. It allows this class
	 * to redirect the content from _bodyFd to the client socket Fd.
	 */
	FlowBuffer&					_bodyBuffer;

	RawResponse();
	RawResponse(const RawResponse& ref);

	RawResponse&	operator=(const RawResponse& ref);
public:
	RawResponse(Response &response, FlowBuffer &bodyBuffer);
	~RawResponse();

	FlowState	sendResponseToSocket(int socketFd);
};

#endif // !RAW_RESPONSE_HPP
