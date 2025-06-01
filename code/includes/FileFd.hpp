#ifndef FILE_FD_HPP
# define FILE_FD_HPP

# include <stdint.h>     // for uint32_t
# include <sys/types.h>  // for mode_t
# include <cerrno>       // for errno
# include <cstdio>       // for size_t, L_tmpnam
# include <exception>    // for exception

# include "AFdData.hpp"  // for AFdData

/**
 * @class FileFd
 * @brief This class represents a classic file, it can not be blocking.
 *
 */
class FileFd : public AFdData
{
private:
	/**
	 * @brief The size of the file when opening it.
	 */
	size_t	_fileSize;

	FileFd(void);
	FileFd(const FileFd &ref);

	FileFd&	operator=(const FileFd& ref);
public:
	FileFd(const char* path, int flags, mode_t mode);
	FileFd(const char* path, int flags);
	~FileFd(void);

	/**
	 * @brief Unused function as this class isn't blocking.
	 * @note If we change the function (epoll, poll, select), this will become usefull
	 * as the non blocking fds will be treated the same as blocking fds.
	 *
	 */
	void			callback(uint32_t events);

	/**
	 * @return The Size when opening the file.
	 */
	size_t			getSize(void) const;
	/**
	 * @brief Returns a new file, or NULL if it fails.
	 * The path of the file is written in the name array.
	 *
	 */
	static FileFd*	getTemporaryFile(char (&name)[L_tmpnam]);

	class FileOpeningError : public std::exception
	{
	public:
		FileOpeningError()
		{
			_errno = errno;
		}

		int getErrno() const
		{
			return (_errno);
		}

		virtual const char* what() const throw()
		{
			return ("Error opening a file");
		}

		virtual ~FileOpeningError() throw() {}
	private:
		int		_errno;
	};
};

#endif // !FILE_FD_HPP
