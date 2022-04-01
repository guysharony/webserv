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

# include "../request/request.hpp"
# include "../response/response.hpp"
# include "../core/message.hpp"

class Client
{
public:
	Client(void);
	Client(int socket_fd);
	Client(int socket_fd, std::string server_addr, int server_port, std::string client_addr, int client_port);
	Client(Client const &src);
	Client	&operator=(Client const &rhs);
	~Client();

	bool		operator==(Client const &rhs);

	// Getters
	std::string const		&getClientAddr(void);
	int					getClientPort(void);
	int					getSocketFd(void);
	std::string const		&getServerAddr(void);
	int					getServerPort(void);

	// Setters
	void					setClientAddr(std::string const &addr);
	void					setClientPort(int port);
	void					setSocketFd(int socket_fd);
	void					setServerAddr(std::string const &addr);
	void					setServerPort(int port);

	// Debug functions
	void					print();

private:
	std::string			_client_addr;	// The IP address of the client
	int					_client_port;	// The port of the server which is connected to the client (the one created by accept, not the one on which the server is listening)
	int					_socket_fd;	// The socket which is used to communicate between client and server
	std::string			_server_addr;	// The IP address of the server
	int					_server_port;	// The port of the server FROM which the client connected (the one on which the server is listening)
	response				_response;
	request				_request; // All of the request/response pairs associated with this client
};

#endif
