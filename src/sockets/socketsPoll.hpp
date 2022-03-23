#ifndef SOCKETS_POLL_HPP
# define SOCKETS_POLL_HPP

# include <iostream>		// std::cout
# include <sys/types.h>		// socket - included for portability
# include <sys/socket.h> 	// socket, AF_LOCAL, SOCK_STREAM, inet_addr
# include <cstring>			// memset
# include <string>			// string
# include <netinet/in.h> 	// sockaddr_in, inet_addr
# include <arpa/inet.h>		// htons, inet_addr
# include <unistd.h>		// close
# include <vector>			// vector
# include <sys/poll.h>		// poll
# include <sys/ioctl.h>
# include <set>			// set
# include <map>			// map
# include <queue>			// queue

# include "../config/config.hpp"

class SocketsPoll
{
	public:
		SocketsPoll(void);
		SocketsPoll(SocketsPoll const & src);
		~SocketsPoll();

		typedef std::vector<pollfd>		pollfd_type;
		typedef size_t					size_type;

		void			append(int fd, short events);

		int			nfds;
		pollfd_type	fds;
};

#endif
