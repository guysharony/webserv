#include "webserv.hpp"

Webserv::Webserv(void)
{ }

Webserv::Webserv(Webserv const & src)
{ *this = src; }

Webserv::~Webserv()
{ }

int			Webserv::load(char *filename)
{ return this->load(std::string(filename)); }

int			Webserv::load(std::string filename)
{
	try {
		if (!this->_config.load(filename))
			return 0;

		for (Config::configuration_type it = this->_config.configuration.begin(); it != this->_config.configuration.end(); it++) {
			this->_sockets.prepare(it->host, toInteger(it->port));
		}

		this->_sockets.initialize();

		return 1;
	} catch (const std::exception& e) {
		std::cout << e.what() << std::endl;
	}

	return 0;
}

bool		Webserv::run(void) {
	std::cout << "Waiting on poll()..." << std::endl;
	this->_sockets.listen();

	std::cout << "test" << std::endl;

	/*
	std::vector<pollfd>::iterator it = this->_sockets.sockets_poll.fds.begin();
	while (it != this->_sockets.sockets_poll.fds.end()) {
		std::cout << std::endl << "pollfd#" << it->fd << "->revents: " << it->revents << std::endl;

		if (this->_sockets.to_listen.count(it->fd) && (it->revents & POLLHUP))
		{
			this->_initialize_listen();
		}
		else if (this->_sockets.to_accept.count(it->fd) && (it->revents & POLLIN))
		{
			this->_initialize_accept();
		}
		else if (this->_sockets.to_recv.count(it->fd) && (it->revents & POLLIN))
		{
			std::string msg = this->_recv_message();
			this->_queue_message("You said: " + msg);
		}
		else if (this->_sockets.to_send.count(it->fd) && (it->revents & POLLOUT))
		{
			int sent = this->_send_next_message();
			(void)sent; // ****
		}

		it++;
	}

	this->_remove_pfds();
	*/

	return this->_run;
}

int			Webserv::_initialize_listen(void)
{
	int fd = this->_sockets.current->fd;

	std::cout << "Listening on fd " << fd << std::endl;
	this->_sockets.to_accept.insert(fd);
	this->_sockets.to_listen.erase(fd);
	return 0;
}

int			Webserv::_initialize_accept(void)
{
	struct sockaddr_in 	cli_addr;
	socklen_t			cli_len;
	int				newsockfd;
	int 				fd;

	fd = this->_sockets.current->fd;
	cli_len = sizeof(cli_addr);

	newsockfd = accept(fd, (struct sockaddr *)&cli_addr, &cli_len);
	if (newsockfd < 0) {
		Message::error("accept() failed.");
		return 0;
	}

	std::cout << "server: got connection from " << inet_ntoa(cli_addr.sin_addr) << " port " << ntohs(cli_addr.sin_port) << std::endl;
	this->_sockets.sockets_poll.append(newsockfd, POLLIN);
	this->_sockets.to_recv.insert(newsockfd);

	fcntl(newsockfd, F_SETFL, O_NONBLOCK);

	exit(0);

	return (0);
}

std::string	Webserv::_recv_message(void)
{
	char 	buffer[256];
	int		newsockfd;
	int		fd;
	int		n;

	fd = this->_sockets.current->fd;

	std::cout << "Got a POLLIN revent on " << fd << std::endl;

	newsockfd = fd;
	bzero(buffer, 256);
	n = recv(newsockfd, buffer, 255, 0);
	if (n < 0) {
		Message::error("socket() failed.");
	}

	if (n == 0)
	{
		std::cout << "Connection on fd " << fd << " closed. Removing pfd" << std::endl;
		this->_sockets.to_recv.erase(fd);
		this->_sockets.to_remove.insert(fd);
	}
	else
	{
		if (std::string(buffer) == "\r\n")
			std::cout << "RECEIVED AN EMPTY LINE" << std::endl;
		else if (std::string(buffer).find("\r\n\r\n"))
			std::cout << "FOUND TWO CONSECUTIVE NEWLINES" << std::endl;
		std::cout << "here is the message: " << buffer << std::endl;
	}

	return (buffer);
}

int			Webserv::_queue_message(std::string msg)
{
	std::cout << "Queueing message: " << msg << std::endl;

	this->_sockets.to_send[this->_sockets.current->fd].push(msg);
	this->_sockets.current->events |= POLLOUT; // Should check that getPollFd returned a valid pollfd (not pfds.end())
	return 0;
}

int			Webserv::_send_next_message(void)
{
	std::basic_string<char>::size_type ret;
	int 							fd;

	ret = 0;
	fd = this->_sockets.current->fd;

	if (this->_sockets.to_send[fd].empty() == 0)
	{
		std::string	next_msg = this->_sockets.to_send[fd].front();
		ret = this->_send_message(next_msg);
		if (ret == next_msg.length())
		{
			this->_sockets.to_send[fd].pop();
		}
		else
		{
			// should truncate message
			Message::error("we should truncate the message, because we didn't send it all");
		}
	}

	if (this->_sockets.to_send[fd].empty() == 1)
	{
		this->_sockets.current->events &= ~(POLLOUT); // Should check that getPollFd returned a valid pollfd (not pfds.end())
	}

	return (ret);
}

std::basic_string<char>::size_type		Webserv::_send_message(std::string & msg)
{
	return send(this->_sockets.current->fd, msg.c_str(), msg.length(), 0);
}

void								Webserv::_remove_pfds(void)
{
	std::set<int>::iterator			current;
	std::set<int>::iterator 			previous;
	std::set<int>::iterator			end;

	current = this->_sockets.to_remove.begin();

	while (current != this->_sockets.to_remove.end())
	{
		this->_remove_pfd(*current);
		previous = current;
		++current;
		this->_sockets.to_remove.erase(previous);
	}
}

void								Webserv::_remove_pfd(int fd)
{
	std::vector<pollfd>::iterator		current;

	current = this->_sockets.sockets_poll.fds.begin();

	while (current != this->_sockets.sockets_poll.fds.end())
	{
		if (current->fd == fd)
		{
			this->_sockets.sockets_poll.fds.erase(current);
			return;
		}

		++current;
	}
}