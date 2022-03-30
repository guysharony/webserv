#include "CGI.hpp"

CGI::CGI(void) : _cgi_path(CGI_DEFAULT_PATH)
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

int	CGI::launch_cgi(std::string const & filename)
{
	int fd[2];
	pid_t pid;

	if (pipe2(fd, O_NONBLOCK))
		Message::error("pipe2() failed");
	Message::debug("Fd: ");
	Message::debug(fd[0]);
	Message::debug("\n");


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

		setenv("PATH_INFO", "test value", true); // Can assign PATH_INFO here once we know what path to use
		char *executable = strdup(CGI_DEFAULT_PATH);
		char *argument = strdup(filename.c_str());
		char * const argv[3] = {executable, argument, NULL};

		execve(executable, argv, environ); //pathname, argv, envp
		Message::error("execve() failed");
	}
	// Parent


	
	close(fd[1]);
	return (fd[0]);
}
