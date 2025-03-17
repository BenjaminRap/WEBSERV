#include "Configuration.hpp"
#include "parsing.hpp"
#include <csignal>                  // for signal, SIG_ERR, SIGINT
#include <cstdlib>                  // for EXIT_FAILURE
#include <exception>                // for exception
#include <iostream>                 // for char_traits, basic_ostream, opera...
#include <string>                   // for basic_string
#include "Configuration.hpp"        // for Configuration
#include "socketCommunication.hpp"  // for getReturnCodeWithSignal, getSigna...
#include "Request.hpp"
#include "Response.hpp"
#include "RawResponse.hpp"

/**
 * @brief Create a mostly uninitialize Configuration and call the function to
 * handle write and read request. It call the handleIoEvents till he receive a signal.
 * @return Return the signal + 128
 */

int	main(void)
{
	try
	{
		Response	r;
		r.setResponseCode(200, "OK");
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	

	// std::string	sLine = "GET /index.html HTTP/1.1\r\n";
	// std::string	sHeader = "1: 1\r\n22: 22\r\n333: 333\r\na: a\r\n\r\n";

	// try
	// {
	// Request	r;

	// 	if (r.parseStatusLine(sLine.c_str(), 26))
	// 	{
	// 		std::cout << "Error parsing statusLine" << std::endl;
	// 		r.reset();
	// 	}
	// 	else if (r.parseHeader(sHeader.c_str(), 32))
	// 	{
	// 		std::cout << "Error parsing header" << std::endl;
	// 		r.reset();
	// 	}
	// 	else
	// 		std::cout << r << std::endl;
	// }
	// catch(const std::exception& e)
	// {
	// 	std::cerr << e.what() << '\n';
	// }


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
