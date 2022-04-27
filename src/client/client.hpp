#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <iostream>
# include <string>
# include <map>
# include <queue>
# include <utility>
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>

# include "../response/response.hpp"

class Client {
	public:
		Client(Config *config, Descriptors *descriptors, int socket_fd);
		~Client();

		bool					operator==(Client const &rhs);


		std::string const		&getClientAddr(void);
		int					getClientPort(void);
		int					getSocketFd(void);
		std::string const		&getServerAddr(void);
		int					getServerPort(void);
		int					getEvent(void);
		std::string			getMethod(void);
		int					getConnection(void);
		int					getStatus(void);
		int					getLine(void);
		int					getEnd(void);
		bool					getClose(void);
		bool					getRequestFailed(void);


		void					setClientAddr(std::string const &addr);
		void					setClientPort(int port);
		void					setSocketFd(int socket_fd);
		void					setServerAddr(std::string const &addr);
		void					setServerPort(int port);
		void					setEvent(int event);
		void					setClose(bool value);

		int					readResponse(STRBinary & packet);
		void					appendRequest(std::vector<char> & packet);
		void					closeResponse(void);

		int					prepareResponse(void);
		int					execute(void);

	private:
		int						_socket_fd;
		Request					_request;
		Response					_response;

		Client(void);
		Client(Client const &src);
		Client					&operator=(Client const &rhs);

		void						log(void);
};

#endif
