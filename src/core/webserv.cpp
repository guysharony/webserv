#include "webserv.hpp"

static int g_sigint = 0;

void signalHandlerInt(int sig)
{
	if (sig == SIGINT)
		g_sigint = 1;
	signal(SIGINT, SIG_DFL);
}

Webserv::Webserv(void)
:
	descriptors(),
	sockets(&descriptors),
	_run(true),
	_compress_array(false)
{ }

Webserv::~Webserv()
{
	this->closeClients();
	this->closeServers();
}


int			Webserv::load(char *filename)
{ return this->load(std::string(filename)); }

int			Webserv::load(std::string const filename)
{
	try {
		if (!this->config.load(filename))
			return 0;

		for (Config::configuration_type it = this->config.configuration.begin(); it != this->config.configuration.end(); it++) {
			for (Config::listen_type::iterator it2 = it->listen.begin(); it2 != it->listen.end(); it2++) {
				for (Config::ports_type::iterator it3 = it2->second.begin(); it3 != it2->second.end(); it3++) {
					this->sockets.prepare(it2->first, toInteger(*it3));
				}
			}
		}

		for (Sockets::socket_listener_type	it = this->sockets.sockets.begin(); it != this->sockets.sockets.end(); it++) {
			this->sockets.initialize(it);
		}

		return 1;
	} catch (const std::exception& e) {
		std::cout << e.what() << std::endl;
	}

	return 0;
}

void		Webserv::closeClients(void) {
	client_type	ite = this->_clients.end();
	for (client_type it = this->_clients.begin(); it != ite; ++it)
		delete (*it);

	this->_clients.clear();
}

void		Webserv::closeServers(void) {
	Descriptors::poll_type ite = this->descriptors.descriptors.end();
	for (Descriptors::poll_type it = this->descriptors.descriptors.begin(); it != ite; ++it) {
		if (this->descriptors.getDescriptorType(it->fd) == "server") {
			close(it->fd);
		}

		this->descriptors.deleteDescriptor(it->fd);
	}
}

bool		Webserv::run(void) {
	signal(SIGINT, &signalHandlerInt);
	signal(SIGPIPE, SIG_IGN);
	while (this->_run) {
		if (g_sigint == 1)
			break;

		if (!this->listen())
			continue;

		for (this->polls_index = 0; this->polls_index < this->polls_size; ++this->polls_index) {
			this->contextInitialize();

			if (this->context.type == "server") {
				if (this->handleServer())
					break;
			} else if (this->context.type == "client") {
				if (this->handleClient())
					break;
			}
		}

		if (this->_compress_array) {
			this->_compress_array = false;
			this->descriptors.compressDescriptors();
		}
	}

	return true;
}

bool			Webserv::listen(void) {
	if (this->sockets.listen() < 0)
		return false;

	this->polls_size = this->descriptors.descriptors.size();

	return true;
}

bool			Webserv::handleServer(void) {
	int		fd;

	if (this->context.poll->revents & POLLIN) {
		if ((fd = this->sockets.accept(this->context.poll->fd)) > 0) {
			this->_clients.push_back(new Client(&this->config, &this->descriptors, fd));
			return true;
		}
	}

	return false;
}

bool			Webserv::handleClient(void) {
	if ((*this->context.client)->getRequestFailed() == true)
	{
		this->_clientReject();
		return false;
	}
	if ((*this->context.client)->getEvent() < EVT_SEND_RESPONSE) {
		if ((this->context.poll->revents & POLLIN) && ((*this->context.client)->getEvent() != EVT_PREPARE_RESPONSE)) {
			if (this->clientReceive() <= 0) {
				this->_clientReject();
				return true;
			}
		}

		if ((*this->context.client)->execute()) {
			this->context.poll->events = POLLOUT;
			(*this->context.client)->setEvent(EVT_SEND_RESPONSE);
		}
	} else if ((*this->context.client)->getEvent() == EVT_SEND_RESPONSE) {
		if (this->context.poll->revents & POLLOUT) {
			STRBinary packet;

			if ((*this->context.client)->readResponse(packet) > 0) {
				if (this->clientSend(packet) <= 0) {
					this->_clientReject();
					return true;
				}
				return false;
			}

			if ((*this->context.client)->getClose()) {
				this->_clientReject();
				return true;
			}

			this->context.poll->events = POLLIN;
			(*this->context.client)->closeResponse();
		}
	}

	return false;
}

/* Context */
bool					Webserv::contextInitialize(void) {
	this->context.poll = this->descriptors.descriptors.begin() + this->polls_index;
	this->context.type = this->descriptors.getDescriptorType(this->context.poll->fd);
	this->context.client = this->_clientFind();

	return this->context.poll->revents != 0;
}

Webserv::client_type	Webserv::_clientFind(void) {
	if (this->context.type != "client")
		return this->_clients.end();

	client_type ite = this->_clients.end();
	for (client_type it = this->_clients.begin(); it != ite; ++it) {
		if ((*it)->getSocketFd() == this->context.poll->fd)
			return it;
	}

	return this->_clients.end();
}

void					Webserv::_clientReject(void) {
	close(this->context.poll->fd);
	this->context.poll->fd = -1;
	this->_compress_array = true;
	this->descriptors.deleteDescriptor(this->context.poll->fd);
	delete (*this->context.client);
	this->_clients.erase(this->context.client);
}

int				Webserv::clientReceive(void) {
	std::vector<char>	packet(5000);
	int				res;

	res = recv(this->context.poll->fd, packet.data(), packet.size(), 0);

	if (res > 0) {
		if (static_cast<int>(packet.size()) > res) {
			packet.resize(res);
		}

		if ((*this->context.client)->getEvent() == NONE)
			(*this->context.client)->setEvent(EVT_REQUEST_LINE);

		(*this->context.client)->appendRequest(packet);
	}

	packet.clear();

	return res;
}

int				Webserv::clientSend(STRBinary value) {
	if (this->context.type != "client")
		return -1;

	int rc = send(this->context.poll->fd, value.c_str(), value.length(), 0);
	if (rc < 0)
		return 0;

	return rc;
}
