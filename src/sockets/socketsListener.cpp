#include "socketsListener.hpp"

SocketsListener::SocketsListener(void)
:
	port(8081),
	server_names(""),
	ip_addr("127.0.0.1")
{ }

SocketsListener::SocketsListener(int port)
:
	port(port),
	server_names(""),
	ip_addr("127.0.0.1")
{ }

SocketsListener::SocketsListener(std::string const & ip_addr)
:
	port(8081),
	server_names(""),
	ip_addr(ip_addr)
{ }

SocketsListener::SocketsListener(std::string const & ip_addr, int port)
:
	port(port),
	server_names(""),
	ip_addr(ip_addr)
{ }

SocketsListener::SocketsListener(std::string const & ip_addr, int port, std::string const & server_names)
:
	port(port),
	server_names(server_names),
	ip_addr(ip_addr)
{ }


SocketsListener::SocketsListener(SocketsListener const &src)
{ *this = src; }

SocketsListener::~SocketsListener()
{ }

SocketsListener		&SocketsListener::operator=(SocketsListener const &rhs)
{
	if (this != &rhs)
	{
		this->port = rhs.port;
		this->ip_addr = rhs.ip_addr;
		this->server_names = rhs.server_names;
	}
	return (*this);
}

std::string const	&SocketsListener::getAddr() const
{ return (this->ip_addr); }

std::string const	&SocketsListener::getServerName() const
{ return (this->server_names); }

in_addr_t			SocketsListener::getInetAddr() const
{
	if (this->ip_addr.empty())
		return INADDR_ANY;

	return (inet_addr(this->getAddr().c_str()));
}

int				SocketsListener::getPort() const
{ return (this->port); }