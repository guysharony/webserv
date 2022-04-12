#include "sockets.hpp"

Sockets::Sockets(void)
{ }

Sockets::Sockets(Sockets const & src)
{ *this = src; }

Sockets::~Sockets()
{ }

void		Sockets::prepare(int port) {
	this->sockets.push_back(SocketsListener(port));
}

void		Sockets::prepare(std::string const & ip_addr) {
	this->sockets.push_back(SocketsListener(ip_addr));
}

void		Sockets::prepare(std::string const & ip_addr, int port) {
	this->sockets.push_back(SocketsListener(ip_addr, port));
}

void		Sockets::prepare(std::string const & ip_addr, int port, std::string const & server_name) {
	this->sockets.push_back(SocketsListener(ip_addr, port, server_name));
}

int		Sockets::initialize(socketsListenerType::iterator	server_iterator) {
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

	if (setsockopt(socketfd, SOL_SOCKET, SO_REUSEPORT, (char *)&enable, sizeof(enable)) < 0)
	{
		close(socketfd);
		Message::error("setsockopt() failed.");
		return -1;
	}

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
		Message::error("bind() failed.");
		close(socketfd);
		return -1;
	}

	if (::listen(socketfd, 100000) < 0)
	{
		Message::error("listen() failed");
		close(socketfd);
		return -1;
	}

	this->sockets_poll.append(socketfd, POLLIN);
	return socketfd;
}

int		Sockets::listen(void)
{ return poll(this->sockets_poll.fds.data(), this->sockets_poll.fds.size(), 0); }

int		Sockets::accept(int fd) {
	int new_client = ::accept(fd, NULL, NULL);

	if (new_client < 0)
		return -1;

	if (fcntl(new_client, F_SETFL, O_NONBLOCK) < 0)
		close(new_client);

	this->sockets_poll.append(new_client, POLLIN);
	return new_client;
}