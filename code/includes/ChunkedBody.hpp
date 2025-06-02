#ifndef CHUNKED_BODY_HPP
# define CHUNKED_BODY_HPP

# include "ABody.hpp"
# include "Request.hpp"


/**
 * @class ChunkedBody
 * @brief An ABody child class that can parse
 * a chunk body and write the unchunked version in a fd.
 *
 */
class ChunkedBody : public ABody
{
private:
	/**
 * @enum ChunkedBodyState
 * @brief An enumeration that deescribes the state of the
 * ChunkedBody. As this class could parse the messsage in
 * mutliple calls, it has to remember where it was the previous
 * time. 
 * @note This ABody SHOULD NOT be used with blocking fd !!
 *
 */
	enum ChunkedBodyState
	{
		SIZE,
		DATA,
		ENDLINE,
		TRAILERS,
		DONE
	};
	/**
	 * @brief The string that ends each line : \r\n
	 */
	static const std::string	_lineEnd;

	/**
	 * @brief The maximum size of this chunked body, it only takes
	 * into account the data of the chunked body, and not the size,
	 * trailers, endLine ...
	 */
	const	size_t				_maxSize;
	/**
	 * @brief The size of the current chunk.
	 * When no chunk has been read, it is set to -1.
	 */
	unsigned long				_chunkSize;
	/**
	 * @brief The state of this request
	 * @ref ChunkedBodyState
	 */
	ChunkedBodyState			_state;

	ChunkedBody();
	ChunkedBody(const ChunkedBody &chunkedBody);
	

	/**
	 * @brief Read the size and \r\n of the chunk data
	 * It sets the _state variable if the size has been read.
	 *
	 * @param begin The beginning of the size
	 * @param end The last character of the buffer
	 * @return The number of character written, or -1  on error.
	 * If -1 is returned, setFinished has been called.
	 * If 0 is returned, it means that the size hasn't been readden
	 * completely.
	 */
	ssize_t	readSize(const char* begin, const char* end);
	/**
	 * @brief Read the data from the buffer and write it into the fd
	 * If the fd is set to -1, it just ignores the body.
	 * It sets the _state variable if the data has been read.
	 *
	 * @param begin The beginning of the data
	 * @param end The end of the buffer
	 * @return The number of character written, or -1  on error.
	 * If -1 is returned, setFinished has been called.
	 */
	ssize_t	writeData(const char* begin, const char* end);
	/**
	 * @brief Read an endline, meaning \r\n.
	 * It sets the _state variable if the endLine has been read.
	 *
	 * @param begin The beginning of the endLine.
	 * @param end The end of the buffer
	 * @return The number of character written, or -1  on error.
	 * If -1 is returned, setFinished has been called.
	 */
	ssize_t	readEndLine(const char* begin, const char* end);
	/**
	 * @brief Read a trailer, it doesn't check if it is well
	 * formatted, neither does it stores it, it is juste ignored.
	 * If the trailer is empty, it changes the _state.
	 *
	 * @param begin The beginning of the endLine.
	 * @param end The end of the buffer 
	 * @return The number of character ignored.
	 */
	ssize_t	readTrailer(const char* begin, const char* end);
	/**
	 * @brief Write a part of the chunked body in the _fd.
	 *
	 * @param begin The beginning of this part of the ChunkedBody.
	 * @param end The end of this part of the ChunkedBody.
	 * @return The number of character written, or -1  on error.
	 * If -1 is returned, setFinished has been called.
	 */
	ssize_t	writeChunkedRequestToFd(const char* begin, const char* end);

	/**
	 * @brief Call the ABody setFinished method and set the _state variable
	 * to CHUNKED_DONE
	 *
	 * @param status 
	 */
	void	setFinished(uint16_t status);
public:
	ChunkedBody(int fd, size_t maxSize, bool isFdBlocking);
	ChunkedBody(size_t maxSize);
	~ChunkedBody();
	
	/**
	 * @brief Write a part of the chunked body in the _fd.
	 *
	 * @param begin The beginning of this part of the ChunkedBody.
	 * @param end The size of this partt of the ChunkedBody.
	 * @return The number of character written, or -1  on error.
	 * If -1 is returned, setFinished has been called.
	 */
	ssize_t		writeToFd(const void* buffer, size_t bufferCapacity);
};

#endif // !CHUNKED_BODY_HPP
