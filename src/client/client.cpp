#include "client.hpp"

Client::Client(Config *config, Descriptors *descriptors, int socket_fd)
:
	_client_addr(),
	_client_port(-1),
	_socket_fd(socket_fd),
	_server_addr(),
	_server_port(-1),
	_request(config, descriptors),
	_response(&_request)
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

void				Client::appendRequest(std::string packet)
{ this->_request.append(packet); }

/* Response */
int				Client::readTemporary(std::string & packet)
{ return this->_request.readTemporary("response", packet); }

int				Client::prepareResponse(void) {
	this->_response.execute();
	this->_response.createBody();
	this->_response.createHeaders();
	this->_response.createResponse();

	this->_request.setEnd(0);

	return 1;
}

void				Client::closeResponse(void)
{
	this->log();
	this->setEvent(NONE);
	this->_request.closeTemporary("request");
	this->_request.closeTemporary("response");
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

	this->prepareResponse();
	return 1;
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
{ std::cout << "\033[0;" << this->getStatusColor() << "m" << this->_response.getHost() << ":" << this->_response.getPort() << " -- [" << getDate("%d/%b/%G %T") << "] -- " << this->_response.getMethod() << " [" << this->_response.getPath() << "] " << this->_response.getStatus() << "\033[0m" << std::endl; }