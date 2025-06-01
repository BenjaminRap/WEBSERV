#ifndef HEADERS_HPP
# define HEADERS_HPP

# include <list>
# include <map>		// for std::map
# include <string>	// for std::string

# define LINE_END "\r\n"
# define LINE_END_LENGTH 2

# define MAX_HEADER_COUNT 30

/**
 * @enum InsertType
 * @brief How to we insert this headder in the headers if
 * there is already a value:
 * 	- CONCAT_COMMA : we concatenate with a comma.
 * 	- CONCAT_SEMICOLON: we concatenate with a semi colon.
 * 	- UNIQUE: we returns an error
 * 	- MUTLIPLE: we keep each one separated in the list.
 * 	- REPLACE: we keep only the last one.
 */
enum	InsertType
{
	CONCAT_COMMA,
	CONCAT_SEMICOLON,
	UNIQUE,
	MULTIPLE,
	REPLACE
};

/**
 * @class Headers
 * @brief This claws represents the Headers of a Request or a Response.
 *
 */
class Headers : public std::map<std::string, std::list<std::string> >
{
private:
	/**
	 * @brief A map with a header name for key, and the Insertion type
	 * for the value.
	 */
	const static std::map<std::string, InsertType>	_headersInsertType;

	Headers(const Headers& ref);

	Headers &operator=(const Headers& ref);

	/**
	 * @return The insertion type of this header.
	 */
	static InsertType		getHeaderInsertType(const std::string& header);
public:
	Headers(void);
	~Headers(void);

	/**
	 * @brief Returns the value associated with the header.
	 * If this header isn't in the map, returns NULL.
	 * @throw If this header has multiple values, throw a std::logic_error
	 *
	 * @param key The header
	 */
	const std::string*		getUniqueHeader(const std::string &key) const;

	/**
	 * @brief Parse a header and adds it into the map.
	 * @note The string should have the \r butt not the \n.
	 *
	 */
	int						parseHeader(const char *line, const char *end);
	/**
	 * @brief Returns the total size of the string representation of the headers.
	 *
	 */
	size_t					getTotalSize(void) const;
	/**
	 * @brief Add this header to the headers.
	 *
	 * @return A bboolean indicating if the function succeed.
	 * It could fail if the header InsertType is unique, and the headers already have one.
	 */
	bool					addHeader(const std::string&key, const std::string& value);
};

std::ostream&	operator<<(std::ostream& o, const Headers& rhs);
std::string&	operator+=(std::string& dest, const Headers& src);

#endif // !HEADERS_HPP
