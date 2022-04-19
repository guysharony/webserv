#include "webserv.hpp"

void		fnExit1(void) {
	std::cout << "Exited..." << std::endl;
}

int		main(int argc, char **argv)
{
	atexit(fnExit1);
	if (argc > 2)
	{
		std::cout << "usage: " << argv[0] << " [config_file]" << std::endl;
		std::cout << "	config_file: Path to a configuration file." << std::endl;
		return (1);
	}

	Webserv		webserv;

	try {
		if (webserv.load(argc > 1 ? argv[1] : "./config/webserv.conf")) {
			webserv.run();
		}
	} catch (const std::exception& e) {
		std::cout << e.what() << std::endl;
	}

	std::cout << "EXIT PROGRAM" << std::endl;
	return (0);
}