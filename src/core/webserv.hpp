#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include <csignal> // signal, SIGINT, SIG_DFL
# include <cstring> // memset
# include <unistd.h>
# include "../formats/temporary.hpp"
# include "../request/request.hpp"
# include "../response/response.hpp"
# include "../client/Client.hpp"
# include "descriptors.hpp"

class Webserv: public virtual Descriptors {
	public:
		Webserv(void);
		~Webserv();

		typedef std::vector<Client*>		clients_type;
		typedef clients_type::iterator	client_type;

		struct 						context_struct
		{
			std::string				type;
			poll_type					poll;
			client_type				client;
		};

		typedef context_struct			context_type;

		int							polls_size;
		int							polls_index;

		Config						config;
		context_type					context;

		int							load(char *filename);
		int							load(std::string const filename);

		bool							run(void);
		bool							listen(void);

		/* Status */
		void							cleanConnections(void);

		/* Events */
		bool							handleServer(void);
		bool							handleClient(void);

		/* Context */
		bool							contextInitialize(void);

		int							clientReceive(void);
		int							clientSend(std::string value);

	private:
		bool							_run;
		clients_type					_clients;
		bool							_compress_array;

		client_type					_clientFind(void);
		void							_clientReject(void);
};

#endif
