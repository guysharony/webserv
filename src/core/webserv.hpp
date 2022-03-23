#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include "../sockets/sockets.hpp"

class Webserv
{
	public:
		Webserv(void);
		Webserv(Webserv const & src);
		~Webserv();

		int			load(char *filename);
		int			load(std::string filename);

		bool			run(void);

		int			current_size;

	private:
		Config		_config;
		Sockets		_sockets;

		bool			_run;
};

#endif