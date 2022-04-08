#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include <csignal> // signal, SIGINT, SIG_DFL
# include <cstring> // memset
# include "context.hpp"

class Webserv: public virtual Context {
	public:
		Webserv(void);
		~Webserv();

		int							load(char *filename);
		int							load(std::string const filename);

		bool							run(void);

	private:
		bool							_listen(void);
		bool							_contextInitialize(void);
		bool							_isServer(void);
		bool							_serverAccept(void);
		bool							_clientRevents(short revents);
		int							_clientReceive(void);
		void 						_clientUpdate(void);
		void							_clientReject(void);
		void							_compress(void);
};

#endif
