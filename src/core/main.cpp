#include "webserv.hpp"


int		main(int argc, char **argv)
{
	if (argc > 2)
	{
		std::cout << "usage: " << argv[0] << " [config_file]" << std::endl;
		std::cout << "	config_file: Path to a configuration file." << std::endl;
		return (1);
	}

	Temporary		temporary;

	temporary.socket(0);
	if (temporary.create(0) && temporary.create(1)) {
		temporary.append(0, "This is a file n°0...");
		temporary.append(1, "This is a file n°1...");

		temporary.cursor(0, 0);
		temporary.cursor(1, 0);

		std::string line;

		std::cout << "___ file n°0 ___" << std::endl;
		while (temporary.read(0, line)) {
			std::cout << "[" << line << "]" << std::endl;
		}

		std::cout << "___ file n°1 ___" << std::endl;
		while (temporary.read(1, line)) {
			std::cout << "[" << line << "]" << std::endl;
		}

		temporary.close(0);
		temporary.close(1);
	}


	/*
	Webserv		webserv;

	try {
		webserv.load(argc > 1 ? argv[1] : "./config/webserv.conf");

		webserv.run();
	} catch (const std::exception& e) {
		std::cout << e.what() << std::endl;
	}
	*/

	return (0);
}