#include "sockets.hpp"

Sockets::Sockets(void)
{ }

Sockets::Sockets(Sockets const & src)
{ *this = src; }

Sockets::~Sockets()
{ }

void		Sockets::prepare(int port) {
	this->_sockets.push_back(SocketsListener(port));
}

void		Sockets::prepare(std::string const & ip_addr) {
	this->_sockets.push_back(SocketsListener(ip_addr));
}

void		Sockets::prepare(std::string const & ip_addr, int port) {
	this->_sockets.push_back(SocketsListener(ip_addr, port));
}

void		Sockets::prepare(std::string const & ip_addr, int port, std::string const & server_name) {
	this->_sockets.push_back(SocketsListener(ip_addr, port, server_name));
}

void		Sockets::initialize(void) {
	for (socketsListenerType::iterator	it = this->_sockets.begin(); it != this->_sockets.end(); it++) {
		this->_initializeSocket(it);
	}
}

bool		Sockets::isListener(int fd) {
	size_t	i;

	i = 0;
	while (i < this->_listener.size()) {
		if (this->_listener[i] == fd)
			return true;
		i++;
	}

	return false;
}

int		Sockets::listen(void) {
	int rc = poll(this->sockets_poll.fds.data(), this->sockets_poll.nfds, 3 * 60 * 1000);

	if (rc < 0) {
		Message::error("poll() failed.");
	}

	if (rc == 0) {
		Message::error("poll() timed out.");
	}

	return rc;
}

void		Sockets::accept(int fd) {
	int new_client = ::accept(fd, NULL, NULL);
	if (new_client < 0)
		return;

	if (fcntl(new_client, F_SETFL, O_NONBLOCK) < 0)
		close(new_client);

	std::cout << "New incoming connection " << new_client << std::endl;

	this->sockets_poll.fds[this->sockets_poll.nfds].fd = new_client;
	this->sockets_poll.fds[this->sockets_poll.nfds].events = POLLIN;
	this->sockets_poll.nfds++;
}

void		Sockets::_initializeSocket(socketsListenerType::iterator socket_iterator) {
	struct sockaddr_in	addr;

	int socketfd = socket(AF_INET, SOCK_STREAM, 0);
	if (socketfd < 0) {
		Message::error("socket() failed.");
		return;
	}

	int enable = 1;
	if (setsockopt(socketfd, SOL_SOCKET,  SO_REUSEADDR, (char *)&enable, sizeof(enable)) < 0)
	{
		close(socketfd);
		Message::error("setsockopt() failed.");
		return;
	}

	if (setsockopt(socketfd, SOL_SOCKET, SO_REUSEPORT, (char *)&enable, sizeof(enable)) < 0)
	{
		close(socketfd);
		Message::error("setsockopt() failed.");
		return;
	}

	if (ioctl(socketfd, FIONBIO, (char *)&enable) < 0)
	{
		close(socketfd);
		Message::error("ioctl() failed.");
		return;
	}

	memset(&addr, 0, sizeof(addr));
	addr.sin_family      = AF_INET;
	addr.sin_addr.s_addr = socket_iterator->getInetAddr();
	addr.sin_port        = htons(socket_iterator->getPort());

	if (bind(socketfd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
	{
		Message::error("bind() failed.");
		close(socketfd);
		return;
	}

	if (::listen(socketfd, 32) < 0)
	{
		Message::error("listen() failed");
		close(socketfd);
		return;
	}

	this->sockets_poll.append(socketfd, POLLIN);
	this->_listener.push_back(socketfd);
}