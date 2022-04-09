#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include <csignal> // signal, SIGINT, SIG_DFL
# include <cstring> // memset
# include <unistd.h>
# include "../formats/temporary.hpp"
# include "../sockets/sockets.hpp"
# include "../request/request.hpp"
# include "../response/response.hpp"
# include "../client/Client.hpp"

class Webserv {
	public:
		Webserv(void);
		~Webserv();

		typedef std::vector<Client*>		clients_type;
		typedef clients_type::iterator	client_type;

		typedef std::vector<pollfd>		polls_type;
		typedef polls_type::iterator		poll_type;

		struct 						context_struct
		{
			bool						is_server;
			bool						is_read;
			bool						is_write;
			poll_type					poll;
			client_type				client;
		};

		typedef context_struct			context_type;

		int							polls_size;
		int							polls_index;

		Config						config;
		context_type					context;
		Sockets						sockets;

		int							load(char *filename);
		int							load(std::string const filename);

		bool							run(void);
		bool							listen(void);
		bool							serverAccept(void);

		int							setConfig(std::string const filename);

		/* Status */
		bool							isRunning(void);
		bool							isServer(int fd);
		void							cleanConnections(void);

		/* Context */
		bool							contextInitialize(void);
		int							contextEvent(void);
		int							contextExecute(void);

		int							clientReceive(void);
		int							clientSend(std::string value);

	private:
		bool							_run;
		clients_type					_clients;
		bool							_close_connection;
		bool							_compress_array;

		client_type					_clientFind(void);
		void							_clientReject(void);

		bool							_listen(void);
		bool							_contextInitialize(void);
		bool							_isServer(void);
		bool							_serverAccept(void);
		bool							_clientRevents(short revents);
		int							_clientReceive(void);
		void 						_clientUpdate(void);
		void							_compress(void);
};

#endif
