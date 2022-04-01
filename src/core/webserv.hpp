#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include <csignal> // signal, SIGINT, SIG_DFL
# include <cstring> // memset
# include "../formats/temporary.hpp"
# include "../sockets/sockets.hpp"
# include "../request/request.hpp"
# include "../response/response.hpp"
# include "../client/Client.hpp"

class Webserv
{
	public:
		Webserv(void);
		Webserv(Webserv const & src);
		~Webserv();

		typedef std::vector<Client>		clients_type;
		typedef clients_type::iterator	client_type;

		int							load(char *filename);
		int							load(std::string const filename);

		bool							run(void);

		int							current_size;
		int							current_index;
		std::vector<pollfd>::iterator		current_iterator;

	private:
		Config						_config;
		Sockets						_sockets;
		clients_type					_clients;
		client_type					_client;
		bool							_close_connection;

		bool							_run;

		bool							_listen(void);
		bool							_contextInitialize(void);
		bool							_isServer(void);
		bool							_serverAccept(void);
		bool							_clientRevents(short revents);
		int							_clientReceive(std::string &packet);
		void 						_clientUpdate(void);
		void							_compress(void);
};

#endif
