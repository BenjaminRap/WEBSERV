#ifndef FILE_FD_HPP
# define FILE_FD_HPP

# include <cerrno>		// for errno
# include <cstdio>		// for L_tmpnam
# include <stdint.h>	// for uint32_t
# include <string>		// for std::string
# include <fcntl.h>		// for mode_t

# include "AFdData.hpp"	// for AFdData


class EPollHandler;

/**
 * @class FileFd
 * @brief This class represents a classic file, it can not be blocking.
 *
 */
class FileFd : public AFdData
{
private:
	size_t	_fileSize;

	FileFd(void);
	FileFd(const FileFd &ref);

	FileFd&	operator=(const FileFd& ref);
public:
	FileFd(const std::string& path, int flags, mode_t mode);
	FileFd(const std::string& path, int flags);
	~FileFd(void);

	/**
	 * @brief Unused function as this class isn't blocking.
	 * @note If we change the function (epoll, poll, select), this will become usefull
	 * as the non blocking fds will be treated the same as blocking fds.
	 *
	 */
	void			callback(uint32_t events);

	size_t			getSize(void) const;
	static FileFd*	getTemporaryFile(char (&name)[L_tmpnam], int rights);

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
