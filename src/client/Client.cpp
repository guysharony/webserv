#include "Client.hpp"

Client::Client(void)
:
	_client_addr(),
	_client_port(-1),
	_socket_fd(-1),
	_server_addr(),
	_server_port(-1),
	_request(),
	_response()
{ }

Client::Client(int socket_fd)
:
	_client_addr(),
	_client_port(-1),
	_socket_fd(socket_fd),
	_server_addr(),
	_server_port(-1),
	_request(),
	_response()
{
	struct sockaddr_in sock_addr;
	struct sockaddr_in peer_addr;

	socklen_t sock_addr_size = sizeof(sock_addr);
	if (getsockname(socket_fd, (struct sockaddr *)&sock_addr, &sock_addr_size) < 0)
		Message::error("getsockname() failed");

	socklen_t peer_addr_size = sizeof(peer_addr);
	if (getpeername(socket_fd, (struct sockaddr *)&peer_addr, &peer_addr_size) < 0)
		Message::error("getpeername() failed");

	this->_client_addr = inet_ntoa(peer_addr.sin_addr);
	this->_client_port = ntohs(peer_addr.sin_port);
	this->_socket_fd = socket_fd;
	this->_server_addr = inet_ntoa(sock_addr.sin_addr);
	this->_server_port = ntohs(sock_addr.sin_port);
}


/* Constructor not used
Client::Client(int socket_fd, std::string server_addr, int server_port, std::string client_addr, int client_port)
:
	_client_addr(client_addr),
	_client_port(client_port),
	_socket_fd(socket_fd),
	_server_addr(server_addr),
	_server_port(server_port),
	_request(),
	_response()
{ }
*/



Client::Client(Client const &src)
{ *this = src; }

Client	&Client::operator=(Client const &rhs)
{
	if (this != &rhs)
	{
		this->_client_addr = rhs._client_addr;
		this->_client_port = rhs._client_port;
		this->_socket_fd = rhs._socket_fd;
		this->_server_addr = rhs._server_addr;
		this->_server_port = rhs._server_port;
		this->_response = rhs._response;
		this->_request = rhs._request;
	}
	return (*this);
}

Client::~Client(void)
{ }


// Operators
bool				Client::operator==(Client const &rhs)
{
	if (this == &rhs)
		return (true);

	if (this->_client_addr == rhs._client_addr
		&& this->_client_port == rhs._client_port
		&& this->_socket_fd == rhs._socket_fd
		&& this->_server_addr == rhs._server_addr
		&& this->_server_port == rhs._server_port)
		return (true);

	return (false);
}

// Getters
std::string const	&Client::getClientAddr(void)
{ return (this->_client_addr); }

int				Client::getClientPort(void)
{ return (this->_client_port); }

int				Client::getSocketFd(void)
{ return (this->_socket_fd); }

std::string const	&Client::getServerAddr(void)
{ return (this->_server_addr); }

int				Client::getServerPort(void)
{ return (this->_server_port); }


// Setters
void				Client::setClientAddr(std::string const &addr)
{ this->_client_addr = addr; }

void				Client::setClientPort(int port)
{ this->_client_port = port; }

void				Client::setSocketFd(int socket_fd)
{ this->_socket_fd = socket_fd; }

void				Client::setServerAddr(std::string const &addr)
{ this->_server_addr = addr; }

void				Client::setServerPort(int port)
{ this->_server_port = port; }

void				Client::setRequest(Config &config)
{ this->_request = request(config); }

void				Client::setResponse(void)
{ this->_response = response(this->_request); }


void				Client::parseRequest(std::string packet)
{ this->_request.parseRequest(packet); }

void				Client::print()
{
	std::cout << "Socket FD: " << this->_socket_fd << "\nServer Addr: " << this->_server_addr
			  << "\tPort: " << this->_server_port << "\nClient Addr: " << this->_client_addr
			  << "\tPort: " << this->_client_port << std::endl;
}
