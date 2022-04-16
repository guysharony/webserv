#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <iostream>
# include <string>
# include <map>
# include <queue>
# include <utility> // pair
# include <sys/types.h>	// socket - included for portability
# include <sys/socket.h> // socket, AF_LOCAL, SOCK_STREAM, inet_addr
# include <netinet/in.h> // sockaddr_in, inet_addr
# include <arpa/inet.h>	// htons, inet_addr

# include "../response/response.hpp"

class Client {
	public:
		Client(Config *config, Descriptors *descriptors, int socket_fd);
		~Client();

		bool					operator==(Client const &rhs);

		// Getters
		std::string const		&getClientAddr(void);
		int					getClientPort(void);
		int					getSocketFd(void);
		std::string const		&getServerAddr(void);
		int					getServerPort(void);
		int					getEvent(void);
		std::string			getMethod(void);
		int					getConnection(void);
		int					getStatus(void);
		int					getLine(void);
		int					getEnd(void);
		bool					getClose(void);

		// Setters
		void					setClientAddr(std::string const &addr);
		void					setClientPort(int port);
		void					setSocketFd(int socket_fd);
		void					setServerAddr(std::string const &addr);
		void					setServerPort(int port);
		void					setEvent(int event);
		void					setClose(bool value);

		int					readResponse(std::string & packet);
		void					appendRequest(std::string packet);
		void					closeResponse(void);

		int					prepareResponse(void);
		int					execute(void);

	private:
		std::string				_client_addr;	// The IP address of the client
		int						_client_port;	// The port of the server which is connected to the client (the one created by accept, not the one on which the server is listening)
		int						_socket_fd;	// The socket which is used to communicate between client and server
		std::string				_server_addr;	// The IP address of the server
		int						_server_port;	// The port of the server FROM which the client connected (the one on which the server is listening)
		Request					_request;		// All of the request/response pairs associated with this client
		Response					_response;

		Client(void);
		Client(Client const &src);
		Client					&operator=(Client const &rhs);

		std::string				getStatusColor(void);
		void						log(void);
};

#endif
