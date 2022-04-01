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
	_run(true)
{ }

Webserv::Webserv(Webserv const & src)
{ *this = src; }

Webserv::~Webserv()
{ }

int			Webserv::load(char *filename)
{ return this->load(std::string(filename)); }

int			Webserv::load(std::string const filename)
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

void 	Webserv::_clientUpdate(void)
{
	client_type it = this->_clients.begin();
	client_type ite = this->_clients.end();
	while (it != ite)
	{
		if ((*it).getSocketFd() == this->current_iterator->fd)
		{
			std::cout << "Client already exists" << std::endl;
			this->_client = it;
			return;
		}
		++it;
	}

	std::cout << "Adding Client" << std::endl;
	this->_clients.push_back(Client(this->current_iterator->fd));
	this->_client = (this->_clients.end() - 1);
	this->_client->setRequest(this->_config);
	return;
}

bool		Webserv::run(void) {
	int	rc;
	bool compress_array;

	rc = 0;
	compress_array = false;

	this->_close_connection = false;
	signal(SIGINT, &signalHandler);
	while (this->_run) {
		if (g_sigint == 1)
			break; // Perhaps we need to shutdown/send messages to active clients first

		if (!this->_listen())
			continue; // Allow server to continue after a failure or timeout in poll

		for (this->current_index = 0; this->current_index != this->current_size; ++this->current_index) {
			if (!this->_contextInitialize())
				continue;

			if (this->_serverAccept())
				break;

			if (this->_clientRevents(POLLIN)) {
				if (this->_clientReceive() <= 0)
					break;

				this->current_iterator->events |= POLLOUT;

				/*
				parsing the request
				request req(this->_config);
				req.parseRequest(packet);
				*/

				/*
				std::cout<< RED <<req<<RESET<<std::endl;
				response res(req);
				try
				{
					req.selectServer();
					res = response(req);
				}
				catch(const Config::ServerNotFoundException& e){
					Message::debug("Server wasn't found: handling error\n");
					req.setRet(STATUS_INTERNAL_SERVER_ERROR);
				}
					res.createResponse();
					//	std::cout<<YELLOW<<res.getResponse()<<RESET<<std::endl;
					send(this->current_iterator->fd, res.getResponse().c_str(), res.getResponse().size(), 0);
					std::cout <<RED<< "Response :" <<RESET<< std::endl;
					std::cout << "[" << GREEN << res.getResponse() << RESET << "]" << std::endl << std::endl;
					this->_close_connection = true;
				
				this->_client->addRequest(req);
				*/
			} else if (this->_clientRevents(POLLOUT)) {
				std::string response;
				response.append("HTTP/1.1 200 OK\r\n");
				response.append("content-length : 11\r\n");
				response.append("content-location : /\r\n");
				response.append("content-type : text/html\r\n");
				response.append("date : Fri, 01 Apr 2022 15:39:15 GMT\r\n");
				response.append("server_name : Michello\r\n");
				response.append("\r\n");
				response.append("all files..\r\n");

				rc = send(this->current_iterator->fd, response.c_str(), response.length(), 0);
				if (rc < 0)
				{
					Message::error("send() failed.");
					this->_close_connection = true;
					break;
				}

				this->current_iterator->events &= ~POLLOUT;
			}

			if (this->_close_connection)
			{
				close(this->current_iterator->fd);
				this->current_iterator->fd = -1;
				compress_array = true;
			}
		}

		if (compress_array) {
			this->_compress();
		}
	}

	return true;
}

bool		Webserv::_listen(void) {
	std::cout << YELLOW << "waiting for a connection..." << RESET << std::endl;

	if (this->_sockets.listen() <= 0)
		return false;

	this->current_size = this->_sockets.sockets_poll.nfds;

	return true;
}

bool		Webserv::_contextInitialize(void) {
	this->current_iterator = this->_sockets.sockets_poll.fds.begin() + this->current_index;

	return this->current_iterator->revents != 0;
}

bool		Webserv::_isServer(void) {
	return this->_sockets.isListener(this->current_iterator->fd);
}

bool		Webserv::_serverAccept(void) {
	if (this->_isServer() && (this->current_iterator->revents & POLLIN)) {
		this->_sockets.accept(this->current_iterator->fd);
		return true;
	}

	return false;
}

bool		Webserv::_clientRevents(short revents) {
	return !this->_isServer() && (this->current_iterator->revents & revents);
}

int		Webserv::_clientReceive(void) {
	char			buffer[BUFFER_SIZE];
	int			res;

	this->_close_connection = false;

	memset(buffer, 0, BUFFER_SIZE);

	res = recv(this->current_iterator->fd, buffer, BUFFER_SIZE, 0);

	if (res == 0) {
		Message::debug("Closing connection: ");
		Message::debug(this->current_iterator->fd);
		Message::debug("\n");

		close(this->current_iterator->fd);
		this->current_iterator->fd = -1;
		this->_close_connection = true;
	} else if (res > 0) {
		std::string packet = std::string(buffer);

		std::cout << RESET << "=== [" << this->current_iterator->fd << "] ===" << std::endl;
		print_buffer(packet, 1000, GREEN);

		this->_clientUpdate();

		this->_client->parseRequest(packet);
	}

	return res;
}

void		Webserv::_compress(void) {
	int	i;
	int	j;

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
