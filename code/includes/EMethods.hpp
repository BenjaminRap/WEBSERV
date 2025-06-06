#ifndef E_METHODS_HPP
# define E_METHODS_HPP

# include <string>

# define METHOD_COUNT 4

/**
 * @brief The availables methods that this http server can manage.
 */
enum EMethods
{
	GET,
	POST,
	DELETE,
	PUT
};

const std::string&	getStringRepresentation(EMethods method);
EMethods			getMethodFromBuffer(const char *str, size_t length);

#endif // !E_METHODS_HPP
