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
		/*
		this->_response = rhs._response;
		this->_request = rhs._request;
		*/
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

int				Client::getConnection(void)
{ return (this->_connection); }

int				Client::getLine(void) {
	this->_current.clear();

	std::size_t	end;

	end = this->_temp.find(CRLF);
	if (end != std::string::npos) {
		this->_current = this->_temp.substr(0, end);
		this->_temp = this->_temp.substr(end + 2);
		return (2);
	}

	if (this->getEvent() < EVT_REQUEST_BODY)
		return (0);

	if (!this->_temp.length() || (this->_temp[this->_temp.length() - 1] == '\r')) {
		if (this->_temp.length()) {
			this->_current = this->_temp.substr(0, this->_temp.length() - 1);
			this->_temp = this->_temp.substr(this->_temp.length() - 1);
		}

		return (0);
	}

	this->_current = this->_temp;
	this->_temp.clear();

	return (1);
}

int				Client::getResponse(std::string &packet)
{ return this->_temporary.read(0, packet); }


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

/*
void				Client::setRequest(Config &config)
{ this->_request = request(config); }

void				Client::setResponse(void)
{ this->_response = response(this->_request); }
*/

void				Client::setEvent(int value)
{ this->_event = value; }


void				Client::appendRequest(std::string packet)
{ this->_temp.append(packet); }

int				Client::appendResponse(std::string packet)
{
	if (this->_temporary.create(0)) {
		this->_temporary.append(0, packet);
		return 1;
	}

	return 0;
}

int				Client::appendRequestBody(std::string packet)
{
	if (this->_temporary.create(1)) {
		this->_temporary.append(1, packet);
		return 1;
	}

	return 0;
}


int				Client::prepareResponse(void) {
	std::cout << "EXECUTE" << std::endl;

	if (this->getMethod() == METHOD_GET) {
		// std::cout << "GET RESPONSE" << std::endl;
		this->appendResponse("HTTP/1.1 200 OK\r\n");
		this->appendResponse("content-length: 11\r\n");
		this->appendResponse("content-location: /\r\n");
		this->appendResponse("content-type: text/html\r\n");
		this->appendResponse("date: Fri, 01 Apr 2022 15:39:15 GMT\r\n");
		this->appendResponse("server_name: Michello\r\n");
		this->appendResponse("\r\n");
		this->appendResponse("all files..");
	} else if (this->getMethod() == METHOD_HEAD) {
		// std::cout << "HEAD RESPONSE" << std::endl;
		this->appendResponse("HTTP/1.1 405 Not Allowed\r\n");
		this->appendResponse("content-length: 11\r\n");
		this->appendResponse("content-location: /\r\n");
		this->appendResponse("content-type: text/html\r\n");
		this->appendResponse("date: Fri, 01 Apr 2022 15:39:15 GMT\r\n");
		this->appendResponse("server_name: Michello\r\n");
		this->appendResponse("\r\n");
	} else {
		// std::cout << "POST RESPONSE" << std::endl;
		this->appendResponse("HTTP/1.1 405 Not Allowed\r\n");
		this->appendResponse("content-length: 11\r\n");
		this->appendResponse("content-location: /\r\n");
		this->appendResponse("content-type: text/html\r\n");
		this->appendResponse("date: Fri, 01 Apr 2022 15:39:15 GMT\r\n");
		this->appendResponse("server_name: Michello\r\n");
		this->appendResponse("\r\n");
		this->appendResponse("all files..");
	}

	this->_temporary.cursor(0, 0);

	return 1;
}

void				Client::displayResponse(void)
{ this->_temporary.display(0); }

void				Client::clearResponse(void)
{ this->_temporary.close(0); }

void				Client::displayRequestBody(void)
{ this->_temporary.display(1); }

void				Client::clearRequestBody(void)
{ this->_temporary.close(1); }

int				Client::execute(void) {
	int	res;

	while ((res = this->getLine()) > 0) {
		if (this->getEvent() == EVT_REQUEST_LINE) {
			Message::debug("REQUEST LINE [" + this->_current + "]\n");
			this->_end = 0;
			this->_encoding = NONE;

			this->_requestLine();
		} else if (this->getEvent() == EVT_REQUEST_HEADERS) {
			if (this->_current.length() == 0) {
				Message::debug("SEPARATOR\n");
				this->_event = EVT_REQUEST_BODY;
				if (this->_encoding == NONE) {
					this->_end = 1;
				}
				continue;
			}

			Message::debug("REQUEST HEADER [" + this->_current + "]\n");

			this->_requestHeaders();
		} else if (this->getEvent() == EVT_REQUEST_BODY) {
			if (this->_encoding == CHUNKED) {
				if (!this->_chunked) {
					if (this->_current.length()) {
						this->_chunk_size = hexToInt(this->_current);
						this->_remaining = this->_chunk_size;
						this->_chunked = true;
						Message::debug("CHUNK SIZE [" + toString(this->_chunk_size) + "]\n");
					}
				} else {
					if (!this->_chunk_size) {
						Message::debug("FINISHED\n");
						this->_end = 1;
						break;
					}

					this->_remaining -= this->_current.length();

					if (this->_remaining > 0 && res == 2) {
						this->_remaining -= 2;
						this->_current.append("\r\n");
					}

					Message::debug("CHUNK BODY [" + this->_current + "]\n");

					this->appendRequestBody(this->_current);

					if (this->_remaining == 0) {
						this->_chunked = false;
					}
				}
			}
		}
	}

	if (this->_end) {
		std::cout << "___ START ___" << std::endl;
		this->displayRequestBody();
		std::cout << "_____________" << std::endl;
		this->prepareResponse();
	}

	return this->_end;
}

int			Client::_requestLine(void)
{
	int			method;
	std::string	target;
	std::string	version;

	if (occurence(this->_current, " ") != 2)
		return (0);

	if (!this->_requestMethod(this->_current, method))
		return (0);

	if (!this->_requestTarget(this->_current, target))
		return (0);

	if (!this->_requestVersion(this->_current, version))
		return (0);

	this->_request_line.method = method;
	this->_request_line.target = target;
	this->_request_line.version = version;

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
