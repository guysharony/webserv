#ifndef SOCKET_LISTENER_HPP
# define SOCKET_LISTENER_HPP

# include <iostream>
# include <string> // string
# include <sys/socket.h> // inet_addr
# include <netinet/in.h> // inet_addr
# include <arpa/inet.h>	// inet_addr

class SocketsListener
{
	public:
		SocketsListener(void);
		SocketsListener(int port);
		SocketsListener(std::string const & ip_addr);
		SocketsListener(std::string const & ip_addr, int port);
		SocketsListener(std::string const & ip_addr, int port, std::string const & server_name);
		SocketsListener(SocketsListener const & src);
		~SocketsListener();

		SocketsListener&	operator=(SocketsListener const & rhs);

		in_addr_t			getInetAddr() const;
		int				getPort() const;
		std::string const	&getAddr() const;
		std::string const	&getServerName() const;

	private:
		int			port;
		std::string	server_name;
		std::string	ip_addr;
};

#endif
