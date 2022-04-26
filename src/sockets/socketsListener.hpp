#ifndef SOCKET_LISTENER_HPP
# define SOCKET_LISTENER_HPP

# include <iostream>
# include <string>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>

class SocketsListener
{
	public:
		SocketsListener(void);
		SocketsListener(int port);
		SocketsListener(std::string const & ip_addr);
		SocketsListener(std::string const & ip_addr, int port);
		SocketsListener(std::string const & ip_addr, int port, std::string const & server_names);
		SocketsListener(SocketsListener const & src);
		~SocketsListener();

		SocketsListener&	operator=(SocketsListener const & rhs);

		in_addr_t			getInetAddr() const;
		int				getPort() const;
		std::string const	&getAddr() const;
		std::string const	&getServerName() const;

	private:
		int			port;
		std::string	server_names;
		std::string	ip_addr;
};

#endif
