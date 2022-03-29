#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include <csignal> // signal, SIGINT, SIG_DFL
# include <cstring> // memset
# include "../formats/temporary.hpp"
# include "../sockets/sockets.hpp"
# include "../request/request.hpp"
# include "../client/Client.hpp"

class Webserv
{
	public:
		typedef std::vector<Client>	clients_container;
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
		clients_container				_clients;

		bool							_run;

		void							_compress(void);

		Client						*updateClient(Client const & client_id);
};

#endif
