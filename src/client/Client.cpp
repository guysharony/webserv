#include "Client.hpp"

Client::Client(void)
:
	_client_addr(),
	_client_port(-1),
	_socket_fd(-1),
	_server_addr(),
	_server_port(-1),
	_event(NONE),
	_encoding(NONE),
	_remaining(-1),
	_temporary(),
	_end(0)
{ }

Client::Client(int socket_fd)
:
	_client_addr(),
	_client_port(-1),
	_socket_fd(socket_fd),
	_server_addr(),
	_server_port(-1),
	_event(NONE),
	_encoding(NONE),
	_remaining(-1),
	_temporary(),
	_end(0)
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
	this->_temporary.socket(this->_socket_fd);
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
		this->_response = rhs._response;
		this->_request = rhs._request;
		this->_temporary = rhs._temporary;
		this->_event = rhs._event;
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

int				Client::getEvent(void)
{ return (this->_event); }

int				Client::getMethod(void)
{ return (this->_request_line.method); }


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

void				Client::setEvent(int value)
{ this->_event = value; }


void				Client::appendRequest(std::string packet)
{ this->_temp.append(packet); }

int				Client::execute(void) {
	std::size_t	found;

	if (this->_event < EVT_REQUEST_BODY)
	{
		while ((found = this->_temp.find("\r\n")) != std::string::npos) {
			this->_current = this->_temp.substr(0, found);
			this->_temp = this->_temp.substr(found + 2);

			if (this->_event == EVT_REQUEST_LINE) {
				this->_end = 0;

				if (!this->_requestLine())
					Message::error("Bad request");
			} else if (this->_event == EVT_REQUEST_HEADERS) {
				if (this->_current.length() == 0) {
					this->_event = EVT_REQUEST_BODY;

					if (this->_encoding == NONE) {
						this->_end = 1;
					}

					return 0;
				}

				this->_requestHeaders();
			}
		}
	} else if (this->_event == EVT_REQUEST_BODY) {
		if (this->_encoding == CHUNKED) {
			while ((found = this->_temp.find("\r\n")) != std::string::npos) {
				this->_current = this->_temp.substr(0, found);

				std::cout << "CHUNKED [" << this->_current << "]" << std::endl;
			}
		}
	}

	return this->_end;
}

int			Client::_requestLine(void)
{
	if (occurence(this->_current, " ") != 2)
		return (0);

	if (!this->_requestMethod(this->_current, this->_request_line.method))
		return (0);

	if (!this->_requestTarget(this->_current, this->_request_line.target))
		return (0);

	if (!this->_requestVersion(this->_current, this->_request_line.version))
		return (0);

	this->_event = EVT_REQUEST_HEADERS;

	return (1);
}

int			Client::_requestMethod(std::string & source, int & dst)
{
	std::string	sep = " ";
	size_t 		pos = source.find(sep);
	std::string	line = source.substr(0, pos);

	source = source.substr(pos + sep.length());

	return (isHttpMethod(line, dst));
}

int			Client::_requestTarget(std::string & source, std::string & dst)
{
	std::string	sep = " ";
	size_t 		pos = source.find(sep);
	std::string	line = source.substr(0, pos);

	source = source.substr(pos + sep.length());

	if (line.length() > 0 && line[0] == '/') {
		dst = line;
		return (1);
	}

	return (0);
}

int			Client::_requestVersion(std::string & source, std::string & dst)
{
	if (!source.compare("HTTP/1.1")) {
		dst = source;
		
		return (1);
	}

	return (0);
}

int			Client::_requestHeaders(void)
{
	std::string key;
	std::string value;

	std::cout << "CURRENT: [" << this->_current << "]" << std::endl;
	std::cout << "TEMP:    [" << this->_temp << "]" << std::endl;

	if (this->_requestHeader(this->_current, key, value)) {
		if (!key.compare("host")) this->_request_headers.host = value;
		else if (!key.compare("connection")) {
			this->_request_headers.connection = value;

			if (!value.compare("close")) this->_connection = CLOSE;
			if (!value.compare("keep-alive")) this->_connection = KEEP_ALIVE;
		}
		else if (!key.compare("accept")) this->_request_headers.accept = value;
		else if (!key.compare("accept-encoding")) this->_request_headers.accept_encoding = value;
		else if (!key.compare("accept-language")) this->_request_headers.accept_language = value;
		else if (!key.compare("content-length")) {
			this->_encoding = LENGTH;
			this->_remaining = toInteger(value);
		}
		else if (!key.compare("transfer-encoding")) {
			if (!value.compare("chunked")) {
				this->_encoding = CHUNKED;
				this->_remaining = 0;
				this->_chunked = 0;
			}
		}
		else {
			this->_request_headers.custom[key] = value;
		}
	}

	return 0;
}

int			Client::_requestHeader(std::string source, std::string & key, std::string & value)
{
	size_t	pos = source.find(":");

	std::string tmp_key;
	std::string tmp_value;

	if (pos != std::string::npos)
	{
		tmp_key = source.substr(0, pos);
		tmp_value = source.substr(pos + 1);

		if (!isTchar(tmp_key))
			return (0);

		trim(tmp_value);

		key = toLowercase(tmp_key);
		value = tmp_value;

		return (1);
	}

	return (0);
}

void				Client::displayRequest(void)
{
	if (this->_temporary.create(0)) {
		this->_temporary.display(0);
	}
}

void				Client::print()
{
	std::cout << "Socket FD: " << this->_socket_fd << "\nServer Addr: " << this->_server_addr
			  << "\tPort: " << this->_server_port << "\nClient Addr: " << this->_client_addr
			  << "\tPort: " << this->_client_port << std::endl;
}
