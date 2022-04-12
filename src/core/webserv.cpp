#include "webserv.hpp"

static int g_sigint = 0;

void signalHandler(int sig)
{
	if (sig == SIGINT)
		g_sigint = 1;
	signal(SIGINT, SIG_DFL); // If user sends signal twice, program will exit immediately
}

Webserv::Webserv(void)
:
	_run(true),
	_close_connection(false),
	_compress_array(false)
{ }

Webserv::~Webserv()
{
	client_type ite = this->_clients.end();
	for (client_type it = this->_clients.begin(); it != ite; ++it)
		delete *it;

	this->_clients.clear();
}


int			Webserv::load(char *filename)
{ return this->load(std::string(filename)); }

int			Webserv::load(std::string const filename)
{
	try {
		if (!this->config.load(filename))
			return 0;

		for (Config::configuration_type it = this->config.configuration.begin(); it != this->config.configuration.end(); it++) {
			this->sockets.prepare(it->host, toInteger(it->port));
		}

		this->sockets.initialize();

		return 1;
	} catch (const std::exception& e) {
		std::cout << e.what() << std::endl;
	}

	return 0;
}


bool		Webserv::run(void) {
	signal(SIGINT, &signalHandler);
	while (this->_run) {
		if (g_sigint == 1)
			break; // Perhaps we need to shutdown/send messages to active clients first

		if (!this->listen())
			continue; // Allow server to continue after a failure or timeout in poll

		for (this->polls_index = 0; this->polls_index < this->polls_size; ++this->polls_index) {
			this->contextInitialize();

			if (this->context.is_server) {
				if (this->context.event == EVT_READ) {
					this->sockets.accept(this->context.poll->fd);
					break;
				}
			} else if (this->context.event == EVT_READ) {
				if ((*this->context.client)->getEvent() == NONE)
					(*this->context.client)->setEvent(EVT_REQUEST_LINE);

				if (this->clientReceive() <= 0)
					break;

				if ((*this->context.client)->execute()) {
					this->context.poll->events = POLLOUT;
					(*this->context.client)->setEvent(EVT_SEND_RESPONSE);
				}
			} else if (this->context.event == EVT_WRITE) {
				if ((*this->context.client)->getEvent() == EVT_SEND_RESPONSE) {
					std::string packet;

					while ((*this->context.client)->getResponse(packet)) {
						std::cout << "SEND [" << packet << "]" << std::endl;
						this->clientSend(packet);
					}

					this->context.poll->events = POLLIN;
					(*this->context.client)->setEvent(NONE);
					(*this->context.client)->clearResponse();
				}
			}
		}

		this->cleanConnections();
	}

	return true;
}

bool			Webserv::listen(void) {
	if (this->sockets.listen() < 0)
		return false;

	std::cout << "POLL" << std::endl;
	this->polls_size = this->sockets.sockets_poll.nfds;

	return true;
}

bool			Webserv::serverAccept(void) {
	if (this->context.is_server && this->context.poll->revents & POLLIN) {
		this->sockets.accept(this->context.poll->fd);
		return true;
	}

	return false;
}

void			Webserv::cleanConnections(void) {
	int	i;
	int	j;

	if (this->_compress_array) {
		for (i = 0; i < this->sockets.sockets_poll.nfds; i++) {
			if (this->sockets.sockets_poll.fds[i].fd == -1) {
				for (j = i; j < this->sockets.sockets_poll.nfds - 1; j++) {
					this->sockets.sockets_poll.fds[j].fd = this->sockets.sockets_poll.fds[j + 1].fd;
				}

				i--;
				this->sockets.sockets_poll.nfds--;
				this->sockets.sockets_poll.fds.pop_back();
			}
		}
	}
}

/* Context */
bool					Webserv::contextInitialize(void) {
	this->context.poll = this->sockets.sockets_poll.fds.begin() + this->polls_index;
	this->context.is_server = this->sockets.isListener(this->context.poll->fd);
	this->context.client = this->_clientFind();
	this->context.event = NONE;
	if (this->context.poll->revents & POLLIN)
		this->context.event = EVT_READ;
	if (this->context.poll->revents & POLLOUT)
		this->context.event = EVT_WRITE;

	return this->context.poll->revents != 0;
}

Webserv::client_type	Webserv::_clientFind(void) {
	if (this->context.is_server)
		return this->_clients.end();

	client_type ite = this->_clients.end();
	for (client_type it = this->_clients.begin(); it != ite; ++it) {
		if ((*it)->getSocketFd() == this->context.poll->fd) {
			Message::debug("Client already exists\n");
			return (it);
		}
	}

	Message::debug("Adding client\n");
	this->_clients.push_back(new Client(this->context.poll->fd));
	return (this->_clients.end() - 1);
}

void					Webserv::_clientReject(void) {
	Message::debug("Closing connection: ");
	Message::debug(this->context.poll->fd);
	Message::debug("\n");

	close(this->context.poll->fd);
	this->context.poll->fd = -1;
	this->_compress_array = true;
	delete (*this->context.client);
	this->_clients.erase(this->context.client);
}

int				Webserv::clientReceive(void) {
	char			buffer[BUFFER_SIZE];
	int			res;

	this->_close_connection = false;

	memset(buffer, 0, BUFFER_SIZE);

	std::cout << "RECV" << std::endl;
	res = recv(this->context.poll->fd, buffer, BUFFER_SIZE - 1, 0);

	if (res == 0)
		this->_clientReject();
	else if (res > 0) {
		std::string packet = std::string(buffer);

		#ifdef DEBUG
			std::cout << RESET << "=== [" << this->context.poll->fd << "] - (" << res << ")" << std::endl;
			print_buffer(packet, 1000, GREEN);
		#endif

		(*this->context.client)->appendRequest(packet);
	}

	return res;
}

int				Webserv::clientSend(std::string value) {
	if (this->context.is_server)
		return -1;

	std::cout << "SEND" << std::endl;
	int rc = send(this->context.poll->fd, value.c_str(), value.length(), 0);
	if (rc < 0)
	{
		Message::error("send() failed.");
		this->_close_connection = true;
		return 0;
	}

	return rc;
}