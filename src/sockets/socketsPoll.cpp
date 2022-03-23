#include "socketsPoll.hpp"

SocketsPoll::SocketsPoll(void)
: nfds(0)
{ }

SocketsPoll::SocketsPoll(SocketsPoll const & src)
{ *this = src; }

SocketsPoll::~SocketsPoll()
{ }

void			SocketsPoll::append(int fd, short events) {
	struct pollfd new_pollfd;
	new_pollfd.fd = fd;
	new_pollfd.events = events;

	this->fds.push_back(new_pollfd);
	this->nfds++;
}