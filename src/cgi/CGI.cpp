#include "CGI.hpp"

CGI::CGI(Request *request)
:
	_request(request),
	_executable(NULL),
	_argument(NULL)
{ }

/*
CGI::CGI(std::string const &cgi_path)
: _cgi_path(cgi_path)
{ }
*/

CGI::CGI(CGI const & src)
:
	_request(src._request),
	_executable(src._executable),
	_argument(src._argument)
{ }

CGI::~CGI(void)
{
	if (this->_executable)
		free(this->_executable);

	if (this->_argument)
		free(this->_argument);
}

CGI &CGI::operator=(CGI const & rhs) {
	if (this != &rhs)
	{
		this->_request = rhs._request;
		this->_executable = rhs._executable;
		this->_argument = rhs._argument;
	}
	return (*this);
}

int	CGI::launch_cgi(std::string const & filename) {
	int		fd[2];
	pid_t	pid;
	int		tmp_fd;

	Config::configuration_type        	server = this->_request->selectServer();
	Config::location_type             	location = this->_request->selectLocation(server);

	Message::debug("Launching CGI: ");
	Message::debugln(server->cgi_path);
	Message::debug("Filename: ");
	Message::debugln(filename);

	if (pipe2(fd, O_NONBLOCK))
		Message::error("pipe2() failed");

	pid = fork();
	if (pid < 0)
		Message::error("fork() failed");
	else if (pid == 0)
	{
		// Child
		clearenv();
		std::map<std::string, std::string>	headers = this->_request->getHeader();
		Message::debugln("Headers");
		for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); ++it)
		{
			std::string key(it->first);
			if (key == "content-type")
				setenv("CONTENT_TYPE", it->second.c_str(), true);
			key = "HTTP_" + key;
			Message::debug(key);
			Message::debug(" = ");
			Message::debugln(it->second.c_str());
			setenv(toUppercase(key).c_str(), it->second.c_str(), true);
		}

		// Open tmp file using TmpFileClass
		// Dup tmp_file_fd to stdin
		std::string content_length = toString(this->_request->sizeTemporary("request"), false);
		
		std::cout << "cgi checking post" << std::endl;
		if (this->_request->getMethod() == "POST") {
			std::cout << "cgi with POST" << std::endl;
			tmp_fd = this->_request->fdTemporary("request");
			int c = 0;
			while (tmp_fd < 0 && c < 10)
			{
				std::cout << "rechecking temp fd == " << tmp_fd << std::endl;
				tmp_fd = this->_request->fdTemporary("request");
				sleep(1);
				++c;
			}
			if (tmp_fd < 0)
				Message::error("invalid temp file");
			else {
				// Dup tmp_fd into pipe's stdin
				lseek(tmp_fd, 0, SEEK_SET);

				if (dup2(tmp_fd, STDIN_FILENO) < 0)
					Message::error("dup2() failed on stdin");
				close(tmp_fd);
				setenv("CONTENT_LENGTH", content_length.c_str(), true);
				// setenv("CONTENT_TYPE", "application/x-www-form-urlencoded", true);
			}
		}

		// Prepare environment for execve
		setenv("SERVER_SOFTWARE", SERVER_NAME, true);						// *** This value should be confirmed
		setenv("SERVER_NAME", server->server_name.c_str(), true);
		setenv("GATEWAY_INTERFACE", "CGI/1.1", true);						// *** This value should be confirmed
		setenv("SERVER_PROTOCOL", "HTTP/1.1", true);
		setenv("SERVER_PORT", this->_request->getPort().c_str(), true);
		setenv("PATH_INFO", filename.c_str(), true);
		setenv("QUERY_STRING", this->_request->getParameters().c_str(), true);
		setenv("PATH_TRANSLATED", filename.c_str(), true);
		setenv("REQUEST_METHOD", this->_request->getMethod().c_str(), true);
		setenv("REDIRECT_STATUS", "200", true);
		setenv("DOCUMENT_ROOT", location->root.c_str(), true);
		setenv("SCRIPT_FILENAME", filename.c_str(), true);

		this->_executable = strdup(server->cgi_path.c_str());
		if (!this->_executable)
			Message::error("strdup() failed");

		this->_argument = strdup(filename.c_str());
		if (!this->_argument)
			Message::error("strdup() failed");

		char * const argv[3] = {this->_executable, this->_argument, NULL};

		// Dup stdout into pipe
		close(fd[0]);
		if (dup2(fd[1], STDOUT_FILENO) < 0)
			Message::error("dup2() failed on pipe");
		close(fd[1]);
		int ret = execve(this->_executable, argv, environ); //pathname, argv, envp
		Message::debug("ret: ");
		Message::debugln(ret);
		Message::debug("errno: ");
		Message::debugln(errno);
		Message::error("execve() failed");
	}

	close(fd[1]);
	return (fd[0]);
}
