#include "client.hpp"

Client::Client(Config *config, Descriptors *descriptors, int socket_fd)
:
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

	this->_request.setClientAddress(inet_ntoa(peer_addr.sin_addr));
	this->_request.setClientPort(ntohs(peer_addr.sin_port));

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

	if (this->_socket_fd == rhs._socket_fd
		&& this->_server_addr == rhs._server_addr
		&& this->_server_port == rhs._server_port)
		return (true);

	return (false);
}

// Getters
std::string const	&Client::getClientAddr(void)
{ return (this->_request.getClientAddress()); }

int				Client::getClientPort(void)
{ return (this->_request.getClientPort()); }

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
{ this->_request.setClientAddress(addr); }

void				Client::setClientPort(int port)
{ this->_request.setClientPort(port); }

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
		std::cout << "METHOD [" << this->_request.getMethod() << "]" << std::endl;
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

void				Client::log(void)
{ std::cout << this->getClientAddr() << " - - [" << getDate("%d/%b/%G:%T %z") << "] \"" << this->_request.getMethod() << " " << this->_request.getURI() << " HTTP/1.1\" " << this->_response.getStatus() << " " << this->_response.getContentLength() << " \"" << this->_request.getReferer() << "\" \"" << this->_request.getUserAgent() << "\"" << std::endl; }