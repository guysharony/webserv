#include "Client.hpp"

Client::Client(void)
:
	_client_addr(),
	_client_port(-1),
	_socket_fd(-1),
	_server_addr(),
	_server_port(-1),
	_request(),
	_response(),
	_event(EVT_REQUEST_LINE),
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
	_request(),
	_response(),
	_event(EVT_REQUEST_LINE),
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


int				Client::appendRequest(std::string packet)
{
	std::size_t	found;
	std::string	current;

	if (this->_temporary.create(0)) {
		this->_temporary.append(0, packet);

		if (this->_event < EVT_REQUEST_BODY) {
			this->_temp.append(packet);

			while ((found = this->_temp.find("\r\n")) != std::string::npos)
			{
				current = this->_temp.substr(0, found);

				if (this->_event == EVT_REQUEST_LINE) {
					this->_event = EVT_REQUEST_HEADERS;
				} else if (this->_event == EVT_REQUEST_HEADERS) {
					std::string key;
					std::string value;

					if (current.length() == 0) {
						this->_event = EVT_REQUEST_BODY;

						if (this->_encoding == NONE) {
							this->_end = 1;
						}

						return this->_end;
					}

					if (this->_preparseHeader(current, key, value))
					{
						if (!key.compare("content-length")) {
							this->_encoding = LENGTH;
							this->_remaining = toInteger(value);
						}
						else if (!key.compare("transfer-encoding")) {
							if (!value.compare("chunked")) {
								this->_encoding = CHUNKED;
								this->_remaining = 0;
							}
						}
					}
				}

				this->_temp = this->_temp.substr(found + 2);
			}
		}

		return 0;
	}

	return -1;
}

int			Client::_preparseHeader(std::string source, std::string & key, std::string & value)
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
