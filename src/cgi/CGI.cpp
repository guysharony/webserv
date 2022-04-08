#include "CGI.hpp"

CGI::CGI(void) : _cgi_path(CGI_DEFAULT_PATH)
{
}

CGI::CGI(std::string const &cgi_path) : _cgi_path(cgi_path)
{
}

CGI::CGI(CGI const & src) : _cgi_path(src._cgi_path)
{
}

CGI::~CGI(void)
{
}

CGI &CGI::operator=(CGI const & rhs)
{
	if (this != &rhs)
	{
		this->_cgi_path = rhs._cgi_path;
	}
	return (*this);
}

int	CGI::launch_cgi(std::string const & filename, request_type &request)
{
	int fd[2];
	pid_t pid;

	Config::configuration_struct        &server = request.selectServer();
	Config::location_type             	location = request.selectLocation(server);

	Message::debug("Launching CGI\n");

	if (pipe2(fd, O_NONBLOCK))
		Message::error("pipe2() failed");
	Message::debug("Fd: ");
	Message::debug(fd[0]);
	Message::debug("\n");

	Message::debug("CGI Path: ");
	Message::debug(this->_cgi_path);
	Message::debug("\n");

	Message::debug("Filename: ");
	Message::debug(filename);
	Message::debug("\n");

	std::map<std::string, std::string>headers = request.getHeader();
	Message::debug("Headers\n");
	for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it)
	{
		Message::debug(it->first);
		Message::debug(" = ");
		Message::debug(it->second);
		Message::debug("\n");
	}


	pid = fork();
	if (pid < 0)
		Message::error("fork() failed");
	else if (pid == 0)
	{
		// Child
		close(fd[0]);
		if (dup2(fd[1], STDOUT_FILENO) < 0)
			Message::error("dup2() failed");
		close(fd[1]);

		// Prepare environment for execve
		setenv("SERVER_SOFTWARE", SERVER_NAME, true);						// *** This value must be agreed on
		setenv("SERVER_NAME", server.server_name.c_str(), true);
		setenv("GATEWAY_INTERFACE", "CGI/1.1", true);						// *** This value should be confirmed

		setenv("SERVER_PROTOCOL", "HTTP/1.1", true);
		setenv("SERVER_PORT", server.port.c_str(), true);
		setenv("PATH_INFO", filename.c_str(), true);
		setenv("PATH_TRANSLATED", filename.c_str(), true);
		setenv("REQUEST_METHOD", request.getMethod().c_str(), true);
		setenv("REDIRECT_STATUS", "200", true);
		setenv("DOCUMENT_ROOT", location->root.c_str(), true);
		setenv("SCRIPT_FILENAME", filename.c_str(), true);

		char *executable = strdup(this->_cgi_path.c_str());
		if (!executable)
			Message::error("strdup() failed");
		char *argument = strdup(filename.c_str());
		if (!argument)
			Message::error("strdup() failed");
		char * const argv[3] = {executable, argument, NULL};

		int ret = execve(executable, argv, environ); //pathname, argv, envp
		std::cout << "ret: " << ret << std::endl;
		std::cout << "errno: " << errno << std::endl;
		Message::error("execve() failed");
	}
		
	close(fd[1]);
	return (fd[0]);
}
