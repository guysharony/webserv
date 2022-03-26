#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include <csignal> // signal, SIGINT, SIG_DFL
# include "../sockets/sockets.hpp"
# include "../request/request.hpp"

class Webserv
{
	public:
		Webserv(void);
		Webserv(Webserv const & src);
		~Webserv();

		int							load(char *filename);
		int							load(std::string const filename);

		bool							run(void);

		int							current_size;
		int							current_index;
		std::vector<pollfd>::iterator		current_iterator;

	private:
		Config						_config;
		Sockets						_sockets;

		bool							_run;

		void							_compress(void);
};

#endif
