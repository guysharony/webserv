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

class Sockets
{
	public:
		Sockets(void);
		Sockets(Sockets const & src);
		~Sockets();

		void		prepare(int port);
		void		prepare(std::string const & ip_addr);
		void		prepare(std::string const & ip_addr, int port);
		void		prepare(std::string const & ip_addr, int port, std::string const & server_name);

		void		initialize(void);
		int		listen(void);
		void		accept(void);

		SocketsPoll::pollfd_type::iterator		current;

		typedef std::vector<SocketsListener>	socketsListenerType;

		SocketsPoll						sockets_poll;
		int								index;

	private:
		socketsListenerType					_sockets;

		void		_initializeSocket(socketsListenerType::iterator socket_iterator);
};

#endif
