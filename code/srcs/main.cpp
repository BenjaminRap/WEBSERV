#include <csignal>                  // for signal, SIG_ERR, SIGINT
#include <cstdlib>                  // for EXIT_FAILURE
#include <exception>                // for exception
#include <iostream>                 // for char_traits, basic_ostream, opera...
#include <string>                   // for basic_string

#include "Configuration.hpp"        // for Configuration
#include "socketCommunication.hpp"  // for getReturnCodeWithSignal, getSigna...

#include "Request.hpp"
/**
 * @brief Create a mostly uninitialize Configuration and call the function to
 * handle write and read request. It call the handleIoEvents till he receive a signal.
 * @return Return the signal + 128
 */

 # include <fstream>

void	ft_readfile(const char *path, std::string &buff)
{
	std::ifstream	file;

	file.open(path, file.in);
	if (file.fail())
		throw (RequestException("Couldn't open file"));
	std::getline(file, buff, '\0');
	file.close();
}

int	main(void)
{
	std::string	sLine = "GET /index.html HTTP/1.1\r\n";
	std::string	sHeader = "key1: value1\r\nkey2: value2\r\n\r\n";
	try
	{
		Request	r;
		if (r.parseStatusLine(sLine.c_str(), 26))
		{
			std::cout << "Error parsing statusLine" << std::endl;
			r.reset();
		}
		else if (r.parseHeader(sHeader.c_str(), 30))
		{
			std::cout << "Error parsing header" << std::endl;
			r.reset();
		}
		else
			std::cout << r << std::endl;
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	

	// if (checkError(std::signal(SIGINT, signalHandler), SIG_ERR, "signal() : ") == SIG_ERR)
	// 	return (EXIT_FAILURE);

	// Configuration	conf;

	// while(getSignalStatus() == NO_SIGNAL)
	// {
	// 	try
	// 	{
	// 		handleIOEvents(conf);
	// 	}
	// 	catch(const std::exception& e)
	// 	{
	// 		std::cerr << e.what() << std::endl;
	// 	}
	// }
	// return (getReturnCodeWithSignal());
}