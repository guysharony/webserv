#include "sockets.hpp"

Sockets::Sockets(Descriptors *descriptors)
: _descriptors(descriptors)
{ }

Sockets::~Sockets()
{ }

void		Sockets::prepare(std::string const & ip_addr, int port) {
	socket_listener_type ite = this->sockets.end();
	for (socket_listener_type it = this->sockets.begin(); it != ite; ++it) {
		if (it->getAddr() == ip_addr && it->getPort() == port)
			return;
	}

	this->sockets.push_back(SocketsListener(ip_addr, port));
}

int		Sockets::initialize(socket_listener_type server_iterator) {
	struct sockaddr_in	addr;

	int socketfd = socket(AF_INET, SOCK_STREAM, 0);
	if (socketfd < 0) {
		Message::error("socket() failed.");
		return -1;
	}

	int enable = 1;
	if (setsockopt(socketfd, SOL_SOCKET,  SO_REUSEADDR, (char *)&enable, sizeof(enable)) < 0)
	{
		close(socketfd);
		Message::error("setsockopt() failed.");
		return -1;
	}

	/*
	if (setsockopt(socketfd, SOL_SOCKET, SO_REUSEPORT, (char *)&enable, sizeof(enable)) < 0)
	{
		close(socketfd);
		Message::error("setsockopt() failed.");
		return -1;
	}
	*/

	if (fcntl(socketfd, F_SETFL, O_NONBLOCK) < 0)
	{
		close(socketfd);
		Message::error("fnctl() failed.");
		return -1;
	}

	memset(&addr, 0, sizeof(addr));
	addr.sin_family      = AF_INET;
	addr.sin_addr.s_addr = server_iterator->getInetAddr();
	addr.sin_port        = htons(server_iterator->getPort());

	if (bind(socketfd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
	{
		Message::bindError(server_iterator->getAddr(), server_iterator->getPort(), errno);
		close(socketfd);
		return -1;
	}

	if (::listen(socketfd, 1000) < 0)
	{
		Message::error("listen() failed");
		close(socketfd);
		return -1;
	}

	this->_descriptors->setDescriptor(socketfd, POLLIN);
	this->_descriptors->setDescriptorType(socketfd, "server");
	return socketfd;
}

int		Sockets::listen(void)
{ return poll(this->_descriptors->descriptors.data(), this->_descriptors->descriptors.size(), -1); }

int		Sockets::accept(int fd) {
	int new_client = ::accept(fd, NULL, NULL);

	if (new_client < 0)
		return -1;

	if (fcntl(new_client, F_SETFL, O_NONBLOCK) < 0)
		close(new_client);

	this->_descriptors->setDescriptor(new_client, POLLIN);
	this->_descriptors->setDescriptorType(new_client, "client");
	return new_client;
}