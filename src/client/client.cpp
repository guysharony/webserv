#include "client.hpp"

Client::Client(Descriptors *descriptors, int socket_fd)
:
	_client_addr(),
	_client_port(-1),
	_socket_fd(socket_fd),
	_server_addr(),
	_server_port(-1),
	_encoding(NONE),
	_content_length(-1),
	_body_size(-1),
	_chunk_size(-1),
	_status(0),
	_temporary()
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
	this->_temporary.setDescriptors(descriptors);
	this->_request.setDescriptors(descriptors);
	this->createTemporary("response");
}

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
		this->_request = rhs._request;
	}
	return (*this);
}

Client::~Client(void)
{ this->closeTemporary("response"); }


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

void				Client::setRequest(Config &config)
{ this->_request = Request(config); }

void				Client::setEvent(int value)
{ this->_request.setEvent(value); }

void				Client::setClose(bool value)
{ this->_request.setClose(value); }

void				Client::appendRequest(std::string packet)
{ this->_request.append(packet); }


/* Temporary */
int				Client::createTemporary(std::string const & filename)
{ return this->_temporary.create(filename); }

int				Client::appendTemporary(std::string const & filename, std::string packet)
{
	if (!(this->_temporary.getEvents(filename) & POLLOUT)) {
		this->_temporary.setEvents(filename, POLLOUT);
		return 0;
	}

	this->_temporary.append(filename, packet);
	return 1;
}

int				Client::readTemporary(std::string const & filename, std::string & packet)
{ return this->_temporary.read(filename, packet); }

int				Client::displayTemporary(std::string const & filename)
{ return this->_temporary.display(filename); }

int				Client::resetCursorTemporary(std::string const & filename)
{ return this->_temporary.resetCursor(filename); };

int				Client::closeTemporary(std::string const & filename)
{ return this->_temporary.close(filename); }


void				Client::pushResponse(std::string const & value)
{ this->_response.push(value); }

int				Client::popResponse(std::string & packet)
{
	packet.clear();

	if (this->_response.size() > 0) {
		packet = this->_response.front();
		this->_response.pop();

		return 1;
	}

	return this->readTemporary("response", packet);
}

/* Response */
int				Client::prepareResponse(void) {
	std::cout << this->_request;

	if (!this->getMethod().compare("GET")) {
		// std::cout << "GET RESPONSE" << std::endl;

		this->pushResponse("HTTP/1.1 200 OK\r\n");
		this->pushResponse("content-length: 31\r\n");
		this->pushResponse("content-location: /\r\n");
		this->pushResponse("content-type: text/html\r\n");
		this->pushResponse("date: Fri, 01 Apr 2022 15:39:15 GMT\r\n");
		this->pushResponse("server: Michello\r\n");
		this->pushResponse("\r\n");
		this->appendTemporary("response", "I'm sending this file content...");
	} else if (!this->getMethod().compare("HEAD")) {
		// std::cout << "HEAD RESPONSE" << std::endl;
		this->pushResponse("HTTP/1.1 405 Not Allowed\r\n");
		this->pushResponse("content-length: 11\r\n");
		this->pushResponse("content-location: /\r\n");
		this->pushResponse("content-type: text/html\r\n");
		this->pushResponse("date: Fri, 01 Apr 2022 15:39:15 GMT\r\n");
		this->pushResponse("server: Michello\r\n");
		this->pushResponse("\r\n");
	} else {
		// std::cout << "POST RESPONSE" << std::endl;
		this->pushResponse("HTTP/1.1 405 Not Allowed\r\n");
		this->pushResponse("content-length: 32\r\n");
		this->pushResponse("content-location: /\r\n");
		this->pushResponse("content-type: text/html\r\n");
		this->pushResponse("date: Fri, 01 Apr 2022 15:39:15 GMT\r\n");
		this->pushResponse("server: Michello\r\n");
		this->pushResponse("\r\n");
		this->appendTemporary("response", "I'm sending this file content...");
	}

	this->resetCursorTemporary("response");
	this->_request.setEnd(0);

	return 1;
}

int				Client::execute(void) {
	if (this->getEvent() <= EVT_REQUEST_BODY)
		this->_request.execute();

	if (!this->_request.getEnd())
		return 0;

	if (this->getEvent() == EVT_REQUEST_BODY) {
		this->setEvent(EVT_PREPARE_RESPONSE);
		/*
		#ifdef DEBUG
			std::cout << "___ BODY [" << toString(this->_content_length) << "] ___" << std::endl;
			this->displayTemporary("request");
			std::cout << "_____________" << std::endl;
		#endif
		*/
		return 0;
	}

	this->prepareResponse();
	return 1;
}