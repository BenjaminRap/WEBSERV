#ifndef E_METHODS_HPP
# define E_METHODS_HPP

# include <string>

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

#endif // !E_METHODS_HPP
