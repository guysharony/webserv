#ifndef CONTEXT_HPP
# define CONTEXT_HPP

# include <unistd.h>
# include "../formats/temporary.hpp"
# include "../sockets/sockets.hpp"
# include "../request/request.hpp"
# include "../response/response.hpp"
# include "../client/Client.hpp"

class Context {

	public:
		Context();
		virtual ~Context();

	protected:
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

		/* Setters */
		int							setConfig(std::string const filename);

		/* Status */
		bool							isRunning(void);
		bool							isServer(int fd);
		bool							listen(void);
		bool							serverAccept(void);
		void							cleanConnections(void);

		/* Context */
		bool							contextInitialize(void);
		int							contextEvent(void);
		int							contextExecute(void);

	private:
		Context(Context const &other);
		Context &operator=(Context const &other);

		bool							_run;
		clients_type					_clients;
		Sockets						_sockets;
		bool							_close_connection;
		bool							_compress_array;

		client_type					_clientFind(void);
		int							_clientReceive(void);
		int							_clientSend(std::string value);
		void							_clientReject(void);

};

#endif