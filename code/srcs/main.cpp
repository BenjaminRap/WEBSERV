#include "Configuration.hpp"
#include "parsing.hpp"

int	main(int argc, char **argv)
{
	Configuration	config;
	std::string		file;

	try
	{
		if (argc > 2)
			throw (NumberOfArgumentException());
		if (argc == 2)
		{
			ft_readfile(argv[1], file);
			parse_file(config, file);
		}
		else if (argc == 1)
		{
		 	ft_readfile(DEFAULT_CONFIG_PATH, file);
			parse_file(config, file);
		}
		std::cout << config << std::endl;
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
}
