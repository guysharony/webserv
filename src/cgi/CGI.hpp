#ifndef C_G_I_HEADER_HPP
#define C_G_I_HEADER_HPP

#include <string>
#include <unistd.h>	   // pipe, fork
#include <fcntl.h>	   // O_NONBLOCK
#include <sys/types.h> // fork
#include <cstring>	   // strdup
#include <cstdlib>	   // setenv
#include <cerrno>	   // errno
#include <map>

#include "../../include/constants.hpp"
#include "../request/request.hpp"
#include "../core/message.hpp"

class CGI
{
public:
	// typedef Request request_type;

	CGI(Request *request);
	CGI(std::string const &cgi_path);
	CGI(CGI const &src);
	~CGI();
	CGI &operator=(CGI const &rhs);

	int launch_cgi(std::string const &filename);

private:
	Request *_request;

	CGI(void);
	int _init_env(std::string const &filename);
	int _redirect_io(int fd[2]);
};

#endif
