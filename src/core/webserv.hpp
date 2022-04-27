#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include <csignal>
# include <cstring>
# include <unistd.h>
# include <cstdio>
# include "../temporary/temporary.hpp"
# include "../client/client.hpp"
# include "../sockets/sockets.hpp"

class Webserv {
	public:
		Webserv(void);
		~Webserv();

		typedef std::vector<Client*>		clients_type;
		typedef clients_type::iterator	client_type;

		struct 						context_struct
		{
			std::string				type;
			Descriptors::poll_type		poll;
			client_type				client;
		};

		typedef context_struct			context_type;

		int							polls_size;
		int							polls_index;

		Config						config;
		context_type					context;
		Descriptors					descriptors;

		Sockets						sockets;

		int							load(char *filename);
		int							load(std::string const filename);

		bool							run(void);
		bool							listen(void);

		/* Status */
		void							closeClients(void);
		void							closeServers(void);

		/* Events */
		bool							handleServer(void);
		bool							handleClient(void);

		/* Context */
		bool							contextInitialize(void);

		int							clientReceive(void);
		int							clientSend(STRBinary value);

	private:
		bool							_run;
		clients_type					_clients;
		bool							_compress_array;

		client_type					_clientFind(void);
		void							_clientReject(void);
};

#endif
