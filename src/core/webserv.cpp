#include "webserv.hpp"

Webserv::Webserv(void)
{ }

Webserv::Webserv(Webserv const & src)
{ *this = src; }

Webserv::~Webserv()
{ }

int			Webserv::load(char *filename)
{ return this->load(std::string(filename)); }

int			Webserv::load(std::string filename)
{
	try {
		return this->_config.load(filename);
	} catch (const std::exception& e) {
		std::cout << e.what() << std::endl;
	}

	return 0;
}