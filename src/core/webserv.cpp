#include "webserv.hpp"

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
	int	rc;
	int	len;
	bool	close_connection;
	bool compress_array;
	bool	is_server;
	char	buffer[2048];

	rc = 0;
	len = 0;
	close_connection = false;
	compress_array = false;

	while (this->_run) {
		std::cout<< YELLOW << "waiting for a connection..."<< RESET<<std::endl;
		if (this->_sockets.listen() <= 0) {
			this->_run = false;
			break;
		}

		this->current_size = this->_sockets.sockets_poll.nfds;

		for (this->current_index = 0; this->current_index != this->current_size; ++this->current_index) {
			this->current_iterator = this->_sockets.sockets_poll.fds.begin() + this->current_index;

			if (this->current_iterator->revents == 0)
				continue;

			is_server = this->_sockets.isListener(this->current_iterator->fd);

			if (is_server && (this->current_iterator->revents & POLLIN)) {
				this->_sockets.accept(this->current_iterator->fd);
			} else if (!is_server && (this->current_iterator->revents & POLLIN)) {
				close_connection = false;

				for (size_t bf = 0; bf <= 2048; bf++)
					buffer[bf] = 0;

				rc = recv(this->current_iterator->fd, buffer, sizeof(buffer), 0);
				if (rc < 0)
					break;

				std::cout <<RESET<< "=== [" << this->current_iterator->fd << "] ===" << std::endl;
				//std::cout << buffer << std::endl;
				//parsing the request
				request req(this->_config);
				req.parseRequest(buffer);
				std::cout<< GREEN <<req<<std::endl;
				if (rc == 0) {
					close(this->current_iterator->fd);
					this->current_iterator->fd = -1;
					close_connection = true;
					break;
				}
			}
			else if (!is_server && (this->current_iterator->revents & POLLOUT))
			{
				len = rc;

				rc = send(this->current_iterator->fd, buffer, len, 0);
				if (rc < 0)
				{
					Message::error("send() failed.");
					close_connection = true;
					break;
				}
			}

			if (close_connection)
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

void		Webserv::_compress(void) {
	int	i;
	int	j;

	for (i = 0; i < this->_sockets.sockets_poll.nfds; i++) {
		if (this->_sockets.sockets_poll.fds[i].fd == -1) {
			for (j = i; j < this->_sockets.sockets_poll.nfds - 1; j++) {
				this->_sockets.sockets_poll.fds[j].fd = this->_sockets.sockets_poll.fds[j+1].fd;
			}

			i--;
			this->_sockets.sockets_poll.nfds--;
			this->_sockets.sockets_poll.fds.pop_back();
		}
	}
}