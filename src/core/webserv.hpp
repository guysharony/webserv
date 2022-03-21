#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include "../config/config.hpp"


class Webserv
{
	public:
		Webserv(void);
		Webserv(Webserv const & src);
		~Webserv();

		int			load(char *filename);
		int			load(std::string filename);

	private:
		bool			_run;
		int			_socket;

		bool			_is_read;
		bool			_is_write;

		Config		_config;
};

#endif