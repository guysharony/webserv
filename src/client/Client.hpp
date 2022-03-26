#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <string>
#include <map>
#include <queue>
#include <utility> // pair
# include <sys/types.h>	// socket - included for portability
# include <sys/socket.h> // socket, AF_LOCAL, SOCK_STREAM, inet_addr
# include <netinet/in.h> // sockaddr_in, inet_addr
# include <arpa/inet.h>	// htons, inet_addr

#include "../request/request.hpp"
// # include "response.hpp"
#include "../core/message.hpp"

class Client
{
public:
	typedef request request_type;
	typedef std::string response_type; // should be Response class
	typedef std::pair<request_type, response_type> pair_type;

	// Using a queue of pairs allows us to handle several requests from the same client while maintaining that each request has a one-to-one relationship with a response.
	typedef std::queue<pair_type> request_container;

	Client(void);
	Client(int socket_fd);
	Client(int socket_fd, std::string server_addr, int server_port, std::string client_addr, int client_port);
	Client(Client const &src);
	~Client(void);
	Client &operator=(Client const &rhs);
	bool	operator==(Client const &rhs);

	// Getters and Setters
	std::string const &getClientAddr(void);
	int getClientPort(void);
	void setClientAddr(std::string const &addr);
	void setClientPort(int port);

	int getSocketFd(void);
	void setSocketFd(int socket_fd);

	std::string const &getServerAddr(void);
	int getServerPort(void);
	void setServerAddr(std::string const &addr);
	void setServerPort(int port);

	// Add, update, and delete the requests
	void addRequest(request_type new_request);	// Add a new request that has just been received by the server
	void updateRequest(std::string const &raw_request); // Update the last request if more data is received by the server
	void deleteRequest();								// Delete a request after response has been sent
	pair_type &getRequest();							// Get next request from queue

	// Debug functions
	void print();

private:
	std::string _client_addr;	 // The IP address of the client
	int _client_port;			 // The port of the server which is connected to the client (the one created by accept, not the one on which the server is listening)
	int _socket_fd;				 // The socket which is used to communicate between client and server
	std::string _server_addr;	 // The IP address of the server
	int _server_port;			 // The port of the server FROM which the client connected (the one on which the server is listening)
	request_container _requests; // All of the request/response pairs associated with this client
};

#endif
