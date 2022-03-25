#include "Client.hpp"

Client::Client(void)
	: _client_addr(),
	  _client_port(-1),
	  _socket_fd(-1),
	  _server_addr(),
	  _server_port(-1),
	  _requests()
{
}

Client::Client(int socket_fd, std::string server_addr, int server_port, std::string client_addr, int client_port)
	: _client_addr(client_addr),
	  _client_port(client_port),
	  _socket_fd(socket_fd),
	  _server_addr(server_addr),
	  _server_port(server_port),
	  _requests()
{
}

Client::Client(Client const &src)
{
	*this = src;
}

Client::~Client(void)
{
}

Client &Client::operator=(Client const &rhs)
{
	if (this != &rhs)
	{
		this->_client_addr = rhs._client_addr;
		this->_client_port = rhs._client_port;
		this->_socket_fd = rhs._socket_fd;
		this->_server_addr = rhs._server_addr;
		this->_server_port = rhs._server_port;
		this->_requests = rhs._requests;
	}
	return (*this);
}

std::string const &Client::getClientAddr(void)
{
	return (this->_client_addr);
}

int Client::getClientPort(void)
{
	return (this->_client_port);
}

void Client::setClientAddr(std::string const &addr)
{
	this->_client_addr = addr;
}

void Client::setClientPort(int port)
{
	this->_client_port = port;
}

int Client::getSocketFd(void)
{
	return (this->_socket_fd);
}

void Client::setSocketFd(int socket_fd)
{
	this->_socket_fd = socket_fd;
}

std::string const &Client::getServerAddr(void)
{
	return (this->_server_addr);
}

int Client::getServerPort(void)
{
	return (this->_server_port);
}

void Client::setServerAddr(std::string const &addr)
{
	this->_server_addr = addr;
}

void Client::setServerPort(int port)
{
	this->_server_port = port;
}

// Add, update, and delete the requests
void Client::addRequest(std::string const &raw_request) // Add a new request that has just been received by the server
{
	request_type new_request;
	new_request.parseRequest(raw_request);
	std::make_pair<request_type, response_type>(new_request, response_type());
}

/* NOT IMPLEMENTED YET
void Client::updateRequest(std::string const &raw_request) // Update the last request if more data is received by the server
{

}
*/

void Client::deleteRequest() // Delete a request after response has been sent
{
	this->_requests.pop();
}

Client::pair_type &Client::getRequest() // Get next request from queue
{
	return (this->_requests.front());
}
