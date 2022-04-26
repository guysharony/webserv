#ifndef SOCKETS_HPP
# define SOCKETS_HPP

# include <iostream>
# include <sys/types.h>
# include <sys/socket.h>
# include <cstring>
# include <string>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <unistd.h>
# include <vector>
# include <sys/poll.h>
# include <sys/ioctl.h>
# include <set>
# include <errno.h>
# include <map>
# include <queue>

# include "../config/config.hpp"
# include "../descriptors/descriptors.hpp"
# include "./socketsListener.hpp"

class Sockets {
	public:
		Sockets(Descriptors *descriptors);
		~Sockets();

		typedef std::vector<SocketsListener>	sockets_listener_type;
		typedef sockets_listener_type::iterator	socket_listener_type;

		void					prepare(std::string const & ip_addr, int port);

		int					listen(void);
		int					accept(int fd);

		bool					isListener(int fd);

		int					initialize(socket_listener_type server_iterator);

		sockets_listener_type	sockets;

	private:
		Descriptors			*_descriptors;
};

#endif
