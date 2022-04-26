#include "CGI.hpp"

CGI::CGI(Request *request)
: _request(request)
{ }

CGI::CGI(CGI const &src)
: _request(src._request)
{ }

CGI::~CGI(void)
{ }

CGI &CGI::operator=(CGI const &rhs)
{
	if (this != &rhs)
	{
		this->_request = rhs._request;
	}
	return (*this);
}

int CGI::_init_env(std::string const &filename)
{
	Config::configuration_type server = this->_request->selectServer();
	Config::location_type location = this->_request->selectLocation(server);
	std::map<std::string, std::string> headers = this->_request->getHeader();

	clearenv();
	setenv("SERVER_SOFTWARE", SERVER_NAME, true);
	setenv("SERVER_NAME", this->_request->getHost().c_str(), true);
	setenv("GATEWAY_INTERFACE", "CGI/1.1", true);
	setenv("SERVER_PROTOCOL", "HTTP/1.1", true);
	setenv("SERVER_PORT", this->_request->getPort().c_str(), true);
	setenv("PATH_INFO", this->_request->getURI().c_str(), true);
	setenv("QUERY_STRING", this->_request->getParameters().c_str(), true);
	setenv("PATH_TRANSLATED", filename.c_str(), true);
	setenv("REQUEST_METHOD", this->_request->getMethod().c_str(), true);
	setenv("REDIRECT_STATUS", "200", true);
	setenv("DOCUMENT_ROOT", location->root.c_str(), true);
	setenv("SCRIPT_FILENAME", filename.c_str(), true);

	setenv("HTTP_CONNECTION", this->_request->getConnection() == KEEP_ALIVE ? "keep-alive" : "close", true);
	setenv("REQUEST_URI", this->_request->getURI().c_str(), true);
	setenv("REMOTE_ADDR", this->_request->getClientAddress().c_str(), true);
	setenv("REMOTE_PORT", toString(this->_request->getClientPort()).c_str(), true);

	for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); ++it)
	{
		std::string key(it->first);
		if (key == "content-type")
			setenv("CONTENT_TYPE", it->second.c_str(), true);
		key = "HTTP_" + key;
		setenv(toUppercase(key).c_str(), it->second.c_str(), true);
	}

	std::string content_length = toString(this->_request->sizeTemporary("request"), false);
	if (this->_request->getMethod() == "POST")
		setenv("CONTENT_LENGTH", content_length.c_str(), true);

	return (0);
}

int CGI::_redirect_io(int fd[2])
{
	int tmp_fd;

	if (this->_request->getMethod() == "POST")
	{
		tmp_fd = this->_request->fdTemporary("request");
		if (tmp_fd < 0)
			return (-1);

		lseek(tmp_fd, 0, SEEK_SET);
		if (dup2(tmp_fd, STDIN_FILENO) < 0)
			return (-1);
		close(tmp_fd);
	}

	close(fd[0]);
	if (dup2(fd[1], STDOUT_FILENO) < 0)
		return (-1);
	close(fd[1]);
	return (0);
}

int CGI::launch_cgi(std::string const &filename)
{
	int fd[2];
	pid_t pid;
	char *executable;
	char *argument;

	Config::configuration_type server = this->_request->selectServer();

	if (pipe2(fd, O_NONBLOCK))
		return (-1);

	pid = fork();
	if (pid < 0)
		return (-1);
	else if (pid == 0)
	{
		this->_init_env(filename);
		if (this->_redirect_io(fd))
		{
			write(fd[1], "Status: 500\r\n\r\nCould not redirect to CGI", 40);
			Message::error("Status: 500\r\n\r\nCould not redirect to CGI");
		}			

		executable = &(server->cgi_path[0]);
		argument = const_cast<char *>(&filename[0]);
		char *const argv[3] = {executable, argument, NULL};

		execve(executable, argv, environ);
		Message::error("Status: 500\r\n\r\nCould not execute CGI");
	}

	close(fd[1]);
	return (fd[0]);
}
