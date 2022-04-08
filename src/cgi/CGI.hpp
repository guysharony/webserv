#ifndef C_G_I_HEADER_HPP
#define C_G_I_HEADER_HPP

#include <string>
#include <unistd.h> // pipe, fork
#include <fcntl.h> // O_NONBLOCK
#include <sys/types.h> // fork
#include <cstring> // strdup
#include <cstdlib> // setenv
#include <cerrno> // errno
#include <map>

#include "../request/request.hpp"
#include "../core/message.hpp"

#define CGI_DEFAULT_PATH "/usr/bin/php-cgi"

class CGI
{
	public:
	typedef Request request_type;

	CGI(void);
	CGI(std::string const &cgi_path);
	CGI(CGI const & src);
	~CGI(void);
	CGI &operator=(CGI const & rhs);

	int	launch_cgi(std::string const & filename, request_type &request);

	private:
	std::string _cgi_path;
};

#endif
