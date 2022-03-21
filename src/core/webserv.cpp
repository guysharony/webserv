#include "webserv.hpp"

Webserv::Webserv(void)
:
	_run(true),
	_socket(0),
	_is_read(false),
	_is_write(false)
{ }

Webserv::Webserv(Webserv const & src)
{ *this = src; }

Webserv::~Webserv()
{ }

int			Webserv::load(char *filename)
{ return this->load(std::string(filename)); }

int			Webserv::load(std::string filename)
{
	if (!this->_config.load(filename))
		return (0);

	return (1);
}