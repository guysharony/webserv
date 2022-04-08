#include "context.hpp"

Context::Context()
:
	_run(true),
	_close_connection(false),
	_compress_array(false)
{ }

Context::~Context()
{ }


/* Setters */
int			Context::setConfig(std::string const filename)
{
	try {
		if (!this->config.load(filename))
			return 0;

		for (Config::configuration_type it = this->config.configuration.begin(); it != this->config.configuration.end(); it++) {
			this->_sockets.prepare(it->host, toInteger(it->port));
		}

		this->_sockets.initialize();

		return 1;
	} catch (const std::exception& e) {
		std::cout << e.what() << std::endl;
	}

	return 0;
}

/* Status */
bool			Context::isRunning(void)
{ return (this->_run); }

bool			Context::isServer(int fd)
{ return this->_sockets.isListener(fd); }

bool			Context::listen(void) {
	if (this->_sockets.listen() < 0)
		return false;

	this->polls_size = this->_sockets.sockets_poll.nfds;

	return true;
}

bool			Context::serverAccept(void) {
	if (this->context.is_server && this->context.poll->revents & POLLIN) {
		this->_sockets.accept(this->context.poll->fd);
		return true;
	}

	return false;
}

void			Context::cleanConnections(void) {
	int	i;
	int	j;

	if (this->_compress_array) {
		for (i = 0; i < this->_sockets.sockets_poll.nfds; i++) {
			if (this->_sockets.sockets_poll.fds[i].fd == -1) {
				for (j = i; j < this->_sockets.sockets_poll.nfds - 1; j++) {
					this->_sockets.sockets_poll.fds[j].fd = this->_sockets.sockets_poll.fds[j + 1].fd;
				}

				i--;
				this->_sockets.sockets_poll.nfds--;
				this->_sockets.sockets_poll.fds.pop_back();
			}
		}
	}
}


/* Context */
bool			Context::contextInitialize(void) {
	usleep(100);

	this->context.poll = this->_sockets.sockets_poll.fds.begin() + this->polls_index;
	this->context.is_server = this->_sockets.isListener(this->context.poll->fd);
	this->context.client = this->_clientFind();
	this->context.is_read = this->context.poll->revents & POLLIN;
	this->context.is_write = this->context.poll->revents & POLLOUT;

	return this->context.poll->revents != 0;
}

int			Context::contextExecute(void) {
	if (this->serverAccept()) {
		return 0;
	} else if (!this->context.is_server) {
		if (this->context.client->getEvent() < EVT_REQUEST) {
			if (this->context.poll->revents & POLLIN) {
				if (this->context.client->getEvent() == NONE)
					this->context.client->setEvent(EVT_REQUEST_LINE);

				if (this->_clientReceive() <= 0)
					return 0;
			}

			if (this->context.client->execute()) {
				std::cout << "EXECUTE" << std::endl;
				this->context.poll->events = POLLOUT;
				this->context.client->setEvent(EVT_PREPARE_RESPONSE);
			}

		} else {
			if (this->context.poll->revents & POLLOUT) {
				std::string response;

				if (this->context.client->getMethod() == METHOD_GET) {
					std::cout << "GET RESPONSE" << std::endl;
					response.append("HTTP/1.1 200 OK\r\n");
					response.append("content-length: 11\r\n");
					response.append("content-location: /\r\n");
					response.append("content-type: text/html\r\n");
					response.append("date: Fri, 01 Apr 2022 15:39:15 GMT\r\n");
					response.append("server_name: Michello\r\n");
					response.append("\r\n");
					response.append("all files..");
				} else {
					std::cout << "POST RESPONSE" << std::endl;
					response.append("HTTP/1.1 405 Not Allowed\r\n");
					response.append("content-length: 11\r\n");
					response.append("content-location: /\r\n");
					response.append("content-type: text/html\r\n");
					response.append("date: Fri, 01 Apr 2022 15:39:15 GMT\r\n");
					response.append("server_name: Michello\r\n");
					response.append("\r\n");
					response.append("all files..");
				}

				int rc = send(this->context.poll->fd, response.c_str(), response.length(), 0);
				if (rc < 0)
				{
					Message::error("send() failed.");
					this->_close_connection = true;
					return 0;
				}

				this->context.poll->events = POLLIN;
				this->context.client->setEvent(NONE);
			}
		}
	}

	return 1;
}


Context::client_type	Context::_clientFind(void) {
	if (this->context.is_server)
		return this->_clients.end();

	client_type ite = this->_clients.end();
	for (client_type it = this->_clients.begin(); it != ite; ++it) {
		if ((*it).getSocketFd() == this->context.poll->fd) {
			Message::debug("Client already exists\n");
			return (it);
		}
	}

	Message::debug("Adding client\n");
	this->_clients.push_back(Client(this->context.poll->fd));
	return (this->_clients.end() - 1);
}

void					Context::_clientReject(void) {
	Message::debug("Closing connection: ");
	Message::debug(this->context.poll->fd);
	Message::debug("\n");

	close(this->context.poll->fd);
	this->context.poll->fd = -1;
	this->_compress_array = true;
	this->context.client = this->_clients.erase(this->context.client);
}

int				Context::_clientReceive(void) {
	char			buffer[BUFFER_SIZE];
	int			res;

	this->_close_connection = false;

	memset(buffer, 0, BUFFER_SIZE);

	res = recv(this->context.poll->fd, buffer, BUFFER_SIZE, 0);

	if (res == 0)
		this->_clientReject();
	else if (res > 0) {
		std::string packet = std::string(buffer);

		std::cout << RESET << "=== [" << this->context.poll->fd << "] - (" << res << ")" << std::endl;
		print_buffer(packet, 1000, GREEN);

		this->context.client->appendRequest(packet);
	}

	return res;
}