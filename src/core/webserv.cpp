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
			for (Config::listen_type::iterator it2 = it->listen.begin(); it2 != it->listen.end(); it2++) {
				for (Config::ports_type::iterator it3 = it2->second.begin(); it3 != it2->second.end(); it3++) {
					this->_sockets.prepare(it2->first, toInteger(*it3));
				}
			}
		}

		this->_sockets.initialize();

		return 1;
	} catch (const std::exception& e) {
		std::cout << e.what() << std::endl;
	}

	return 0;
}

Client *Webserv::updateClient(Client const &client_id)
{
	Webserv::clients_container::iterator it = this->_clients.begin();
	Webserv::clients_container::iterator ite = this->_clients.end();
	while (it != ite)
	{
		if (*it == client_id)
		{
			std::cout << "Client already exists" << std::endl;
			return (&(*it));
		}
		++it;
	}
	std::cout << "Adding Client" << std::endl;
	this->_clients.push_back(client_id);
	it = this->_clients.end() - 1;
	return (&(*it));
}

bool		Webserv::run(void) {
	int	rc;
	int	len;
	bool	close_connection;
	bool compress_array;
	bool	is_server;
	char	buffer[BUFFER_SIZE];

	rc = 0;
	len = 0;
	close_connection = false;
	compress_array = false;

	/* CGI Demo
	CGI new_cgi("/usr/bin/php-cgi");							// Create a new CGI connection with the path to CGI
	int pipefd = new_cgi.launch_cgi("www/php/index.php");		// Launch the CGI command with the path to requested file (returns file descriptor to read)
	this->_sockets.sockets_poll.append_pipe(pipefd, POLLIN);	// Add the new file descriptor to poll
	*/

	signal(SIGINT, &signalHandler);
	while (this->_run) {
		if (g_sigint == 1)
			break; // Perhaps we need to shutdown/send messages to active clients first
	//	std::cout<< YELLOW << "waiting for a connection..."<< RESET<<std::endl;
		if (this->_sockets.listen() <= 0) {
			continue ; // Allow server to continue after a failure or timeout in poll
		}

		this->current_size = this->_sockets.sockets_poll.nfds;

		for (this->current_index = 0; this->current_index != this->current_size; ++this->current_index) {
			this->current_iterator = this->_sockets.sockets_poll.fds.begin() + this->current_index;

			if (this->current_iterator->revents == 0)
				continue;

			is_server = this->_sockets.isListener(this->current_iterator->fd);

			if (is_server && (this->current_iterator->revents & POLLIN)) {
				this->_sockets.accept(this->current_iterator->fd);
				break ;
			} else if (this->_sockets.sockets_poll.pipe_fds.count(this->current_iterator->fd) && (this->current_iterator->revents & POLLIN)) { // Handle cgi response here
				Message::debug("Reading from CGI pipe\n");
				memset(buffer, 0, BUFFER_SIZE);
				rc = read(this->current_iterator->fd, buffer, BUFFER_SIZE);
				if(rc < 0)
					Message::error("read() failed");
				Message::debug(buffer);
				Message::debug("\n");

				// Create response from cgi buffer
				// response(client->request, buffer);
				// queue response

				if (rc == 0) {
					close(this->current_iterator->fd);
					this->_sockets.sockets_poll.pipe_fds.erase(this->current_iterator->fd);
					this->current_iterator->fd = -1;
					close_connection = true;
					break;
				}
			} else if (!is_server && (this->current_iterator->revents & POLLIN)) {
				close_connection = false;

				memset(buffer, 0, BUFFER_SIZE);

				rc = recv(this->current_iterator->fd, buffer, BUFFER_SIZE, 0);
				if (rc < 0)
					break;
				if (rc == 0) {
					Message::debug("Closing connection: ");
					Message::debug(this->current_iterator->fd);
					Message::debug("\n");
					close(this->current_iterator->fd);
					this->current_iterator->fd = -1;
					close_connection = true;
					break;
				}
				Client client_id(this->current_iterator->fd);
				client_id.print();
				Client *client;
				client = this->updateClient(client_id);

				std::cout <<RESET<< "=== [" << this->current_iterator->fd << "] ===" << std::endl;
				print_buffer(buffer, 1000, GREEN);
				//parsing the request
				Request req(this->_config);
				req.parseRequest(buffer);
				Response res(req);
				if (req.getRet() < STATUS_BAD_REQUEST){
					Config::configuration_struct server;
					try
					{
						server =req.selectServer();		
					}
					catch(const Config::ServerNotFoundException& e){
						Message::debug("Server wasn't found: handling error\n");
						req.setRet(STATUS_BAD_REQUEST);
					}
					char buffer[2000];
					if (req.isCgi(server)){
						CGI new_cgi("/usr/bin/php-cgi");								// Create a new CGI connection with the path to CGI
					//	 std::cout << RED<<"***********"<<server.root + req.getPath()<<RESET<<std::endl;
						int pipefd = new_cgi.launch_cgi(res.getLoc()->root + req.getPath(), req);			// Launch the CGI command with the path to requested file (returns file descriptor to read)
						this->_sockets.sockets_poll.append_pipe(pipefd, POLLIN);	// Add the new file descriptor to poll
						sleep(2);
						read(pipefd, (void*)buffer, 2000);
						std::cout << buffer <<std::endl;	
						CgiParser  cgiRes;
						cgiRes.parseCgiBuffer(buffer);
						res.createCgiResponse(cgiRes);
					}
					else
						res.createResponse();
				}
				 else
					res.createResponse();
					// queue response instead of sending directly
				send(this->current_iterator->fd, res.getResponse().c_str(), res.getResponse().size(), 0);
				std::cout <<RED<< "Response :" <<RESET<< std::endl;
				std::cout << "[" << GREEN << res.getResponse() << RESET << "]" << std::endl << std::endl;
				client->addRequest(req);
			}
			else if (!is_server && (this->current_iterator->revents & POLLOUT))
			{
				len = rc;

				// buffer = request's buffer
				// send queued response
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
