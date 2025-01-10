#include "Configuration.hpp"

int	main(int argc, char **argv)
{
	try
	{
		if (argc > 2)
			throw (Configuration::NumberOfArgumentException());
		if (argc == 2)
			Configuration	config(argv[1]);
		else if (argc == 1)
		 	Configuration	config(DEFAULT_CONFIG_PATH);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
}