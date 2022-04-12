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
# include <map>		// map
# include <queue>		// queue

# include "../config/config.hpp"
# include "./socketsListener.hpp"
# include "./socketsPoll.hpp"

class Sockets {
	public:
		Sockets(void);
		Sockets(Sockets const & src);
		~Sockets();

		SocketsPoll::pollfd_type::iterator		current;

		typedef std::vector<SocketsListener>	socketsListenerType;

		void		prepare(int port);
		void		prepare(std::string const & ip_addr);
		void		prepare(std::string const & ip_addr, int port);
		void		prepare(std::string const & ip_addr, int port, std::string const & server_name);

		int		listen(void);
		int		accept(int fd);

		bool		isListener(int fd);

		int		initialize(socketsListenerType::iterator server_iterator);

		SocketsPoll						sockets_poll;
		socketsListenerType					sockets;
		int								index;

	private:
		std::set<int>						_listener;
};

#endif
