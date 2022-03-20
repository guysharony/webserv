#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include "config.hpp"


class Webserv
{
	public:
		Webserv(void);
		Webserv(Webserv const & src);
		~Webserv();

		int			load(char *filename);
		int			load(std::string filename);

	private:
		Config		_config;
};

#endif