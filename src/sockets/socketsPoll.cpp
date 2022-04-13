#include "socketsPoll.hpp"

SocketsPoll::SocketsPoll(void)
: nfds(0)
{ }

SocketsPoll::SocketsPoll(SocketsPoll const & src)
{ *this = src; }

SocketsPoll::~SocketsPoll()
{ }

void			SocketsPoll::append(int fd, short events) {
	pollfd* new_pollfd = new pollfd;
	new_pollfd->fd = fd;
	new_pollfd->events = events;

	std::cout << "CREATE [" << fd << "]" << std::endl;

	this->fds.push_back(*new_pollfd);
	this->nfds++;

	for (int i = 0; i < this->nfds; i++) {
		std::cout << RED << "FILE [" << this->fds[i].fd << "]" << RESET << std::endl;
	}

	delete new_pollfd;
}