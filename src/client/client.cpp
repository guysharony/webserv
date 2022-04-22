#include "client.hpp"

Client::Client(Config *config, Descriptors *descriptors, int socket_fd)
:
	_client_addr(),
	_client_port(-1),
	_socket_fd(socket_fd),
	_server_addr(),
	_server_port(-1),
	_request(config, descriptors),
	_response(&_request, descriptors)
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

int				Client::getEvent(void)
{ return (this->_request.getEvent()); }

std::string		Client::getMethod(void)
{ return (this->_request.getMethod()); }

int				Client::getConnection(void)
{ return (this->_request.getConnection()); }

int				Client::getStatus(void)
{ return (this->_request.getStatus()); }

int				Client::getEnd(void)
{ return (this->_request.getEnd()); }

bool				Client::getClose(void)
{ return (this->_request.getClose()); }


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

void				Client::setEvent(int value)
{ this->_request.setEvent(value); }

void				Client::setClose(bool value)
{ this->_request.setClose(value); }

void				Client::appendRequest(std::vector<char> & packet)
{ this->_request.append(packet); }

/* Response */
int				Client::readResponse(STRBinary & packet)
{ return this->_response.readResponse(packet); }

int				Client::prepareResponse(void) {
	if (!this->_response.execute()) {
		this->log();
		this->_request.eventTemporary("body", POLLIN);
		this->_request.setEnd(0);
		return 1;
	}

	return 0;
}

void				Client::closeResponse(void)
{
	this->setEvent(NONE);
	this->_request.closeTemporary("request");
	this->_request.closeTemporary("body");
}

int				Client::execute(void) {
	if (this->getEvent() <= EVT_REQUEST_BODY)
		this->_request.execute();

	if (!this->_request.getEnd())
		return 0;

	if (this->getEvent() == EVT_REQUEST_BODY) {
		this->setEvent(EVT_PREPARE_RESPONSE);
		return 0;
	}

	return this->prepareResponse();
}

std::string		Client::getStatusColor(void) {
	if (this->_response.getStatus() == STATUS_CREATED)
		return "34";

	if (this->_response.getStatus() == STATUS_NOT_FOUND)
		return "33";

	if ((this->_response.getStatus() == STATUS_BAD_REQUEST)
		|| (this->_response.getStatus() == STATUS_INTERNAL_SERVER_ERROR)
		|| (this->_response.getStatus() == STATUS_NOT_ALLOWED)
		|| (this->_response.getStatus() == STATUS_FORBIDDEN))
		return "31";

	return "37";
}

void				Client::log(void)
{
	// std::cout << '172.17.0.1 - - [22/Apr/2022:07:48:49 +0000] "GET /favicon.ico HTTP/1.1" 404 555 "http://www.localhost:8081/" "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/100.0.4896.127 Safari/537.36"' << std::endl;
	std::cout << this->getClientAddr() << " - - [" << getDate("%d/%b/%G:%T %z") << "] \"" << this->_response.getMethod() << " " << this->_response.getURI() << " HTTP/1.1\" " << this->_response.getStatus() << " " << this->_response.getContentLength() << " \"" << this->_request.getReferer() << "\" \"" << this->_request.getUserAgent() << "\"" << std::endl;
	// std::cout << "\033[0;" << this->getStatusColor() << "m" << this->_response.getHost() << ":" << this->_response.getPort() << " -- [" << getDate("%d/%b/%G %T") << "] -- " << this->_response.getMethod() << " [" << this->_response.getURI() << "] " << this->_response.getStatus() << "\033[0m" << std::endl;
}