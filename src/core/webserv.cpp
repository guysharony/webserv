#include "webserv.hpp"

static int g_sigint = 0;

void signalHandler(int sig)
{
	if (sig == SIGINT)
		g_sigint = 1;
	signal(SIGINT, SIG_DFL); // If user sends signal twice, program will exit immediately
}

Webserv::Webserv(void)
{ }

Webserv::~Webserv()
{ }


int			Webserv::load(char *filename)
{ return this->load(std::string(filename)); }

int			Webserv::load(std::string const filename)
{ return this->setConfig(filename); }


bool		Webserv::run(void) {
	signal(SIGINT, &signalHandler);
	while (this->isRunning()) {
		if (g_sigint == 1)
			break; // Perhaps we need to shutdown/send messages to active clients first

		if (!this->listen())
			continue; // Allow server to continue after a failure or timeout in poll

		for (this->polls_index = 0; this->polls_index != this->polls_size; ++this->polls_index) {
			if (!this->contextInitialize())
				continue;

			if (!this->contextExecute())
				break;
		}

		this->cleanConnections();
	}

	return true;
}
