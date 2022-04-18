#ifndef SOCKETS_HPP
# define SOCKETS_HPP

# include <iostream>	// std::cout
# include <sys/types.h>	// socket - included for portability
# include <sys/socket.h> // socket, AF_LOCAL, SOCK_STREAM, inet_addr
# include <cstring>		// memset
# include <string>		// string
# include <netinet/in.h> // sockaddr_in, inet_addr
# include <arpa/inet.h>	// htons, inet_addr
# include <unistd.h>	// close
# include <vector>		// vector
# include <sys/poll.h>	// poll
# include <sys/ioctl.h>
# include <set>		// set
# include <errno.h>
# include <map>		// map
# include <queue>		// queue

# include "../config/config.hpp"
# include "../descriptors/descriptors.hpp"
# include "./socketsListener.hpp"

class Sockets {
	public:
		Sockets(Descriptors *descriptors);
		~Sockets();

		typedef std::vector<SocketsListener>	sockets_listener_type;
		typedef sockets_listener_type::iterator	socket_listener_type;

		void					prepare(int port);
		void					prepare(std::string const & ip_addr);
		void					prepare(std::string const & ip_addr, int port);
		void					prepare(std::string const & ip_addr, int port, std::string const & server_name);

		int					listen(void);
		int					accept(int fd);

		bool					isListener(int fd);

		int					initialize(socket_listener_type server_iterator);

		sockets_listener_type	sockets;

	private:
		Descriptors			*_descriptors;
};

#endif
