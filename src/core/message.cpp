#include "message.hpp"

Message::Message(void)
: _message() { }

Message::Message(std::string const & message)
: _message(message) { }

Message::Message(Message const & message)
{ *this = message; }

Message::~Message()
{ }

void	Message::error(std::string const & message)
{ throw Message::ErrorException(message); }

void	Message::bindError(std::string host, int port, int error)
{
	std::string message;

	if (error == 13)
		message.append("(Connection to this socket is forbidden)");
	else if (error == 48 || error == 98)
		message.append("(Address is already in use)");
	else
		message.append("(" + toString(error) + ")");
	
	throw Message::ErrorException("bind() to " + host + ":" + toString(port) + " failed " + message);
}