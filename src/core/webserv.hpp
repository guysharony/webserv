#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include "../sockets/sockets.hpp"

class Webserv
{
	public:
		Webserv(void);
		Webserv(Webserv const & src);
		~Webserv();

		int			load(char *filename);
		int			load(std::string filename);

		bool			run(void);

		int			current_size;

	private:
		Config		_config;
		Sockets		_sockets;

		bool									_run;

		bool									_execute(void);
		int									_initialize_listen(void);
		int									_initialize_accept(void);
		std::string							_recv_message(void);
		int									_queue_message(std::string msg);
		int									_send_next_message(void);
		std::basic_string<char>::size_type			_send_message(std::string & msg);
		void									_remove_pfds(void);
		void									_remove_pfd(int fd);
};

#endif