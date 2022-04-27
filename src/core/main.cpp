#include "webserv.hpp"

int		main(int argc, char **argv)
{
	srand(time(NULL));

	std::string path = argc > 1 ? toString(argv[1]) : "./config/webserv.conf";

	if (argc > 2 || !isFile(path))
	{
		std::cout << "usage: " << argv[0] << " [config_file]" << std::endl;
		std::cout << "	config_file: Path to a configuration file." << std::endl;
		return (1);
	}

	Webserv		webserv;

	try {
		if (webserv.load(path)) {
			webserv.run();
		}
	} catch (const std::exception& e) {
		std::cout << e.what() << std::endl;
	}

	return (0);
}