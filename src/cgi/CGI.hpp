#ifndef C_G_I_HEADER_HPP
#define C_G_I_HEADER_HPP

#include <string>
#include <unistd.h> // pipe, fork
#include <fcntl.h> // O_NONBLOCK
#include <sys/types.h> // fork
#include <cstring> // strdup
#include <cstdlib> // setenv

#include "../core/message.hpp"

#define CGI_DEFAULT_PATH "/usr/bin/php-cgi"

class CGI
{
	public:
	CGI(void);
	CGI(CGI const & src);
	~CGI(void);
	CGI &operator=(CGI const & rhs);

	int	launch_cgi(std::string const & filename);

	private:
	std::string _cgi_path;
};

#endif
