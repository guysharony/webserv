#include "webserv.hpp"

Webserv::Webserv(void)
{ signal(SIGPIPE, SIG_IGN); }

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

	/*

	CREATING SERVER
	
	*/

	return (1);
}