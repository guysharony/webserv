#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <iostream>
# include <string>
# include <map>
# include <queue>
# include <utility> // pair
# include <sys/types.h>	// socket - included for portability
# include <sys/socket.h> // socket, AF_LOCAL, SOCK_STREAM, inet_addr
# include <netinet/in.h> // sockaddr_in, inet_addr
# include <arpa/inet.h>	// htons, inet_addr

# include "../temporary/temporary.hpp"
# include "../request/request.hpp"
# include "../response/response.hpp"

class Client {
	public:
		Client(Descriptors *descriptors, int socket_fd);
		Client(Client const &src);
		Client				&operator=(Client const &rhs);
		~Client();

		bool					operator==(Client const &rhs);

		// Getters
		std::string const		&getClientAddr(void);
		int					getClientPort(void);
		int					getSocketFd(void);
		std::string const		&getServerAddr(void);
		int					getServerPort(void);
		int					getEvent(void);
		int					getMethod(void);
		int					getConnection(void);
		int					getStatus(void);
		int					getLine(void);
		int					getEnd(void);
		bool					getClose(void);

		// Setters
		void					setClientAddr(std::string const &addr);
		void					setClientPort(int port);
		void					setSocketFd(int socket_fd);
		void					setServerAddr(std::string const &addr);
		void					setServerPort(int port);
		void					setRequest(Config &config);
		void					setResponse(void);
		void					setEvent(int event);
		void					setClose(bool value);

		// Debug functions
		void					print();

		void					appendRequest(std::string packet);
		int					createTemporary(std::string const & filename);
		int					readTemporary(std::string const & filename, std::string &packet);
		int					appendTemporary(std::string const & filename, std::string packet);
		int					displayTemporary(std::string const & filename);
		int					resetCursorTemporary(std::string const & filename);
		int					closeTemporary(std::string const & filename);
		void					pushResponse(std::string const & value);
		int					popResponse(std::string & packet);
		int					prepareResponse(void);
		int					execute(void);

		/* request line */
		struct 								request_line_struct
		{
			int								method;
			std::string						target;
			std::string						version;
		};
		
		typedef	request_line_struct				request_line_type;


		/* request headers */
		struct 								request_headers_struct
		{
			std::string						host;
			std::string						connection;
			std::string						accept;
			std::string						accept_encoding;
			std::string						accept_language;
			std::map<std::string, std::string>		custom;
		};
		
		typedef	request_headers_struct			request_headers_type;

	private:
		Client(void);

		std::string				_client_addr;	// The IP address of the client
		int						_client_port;	// The port of the server which is connected to the client (the one created by accept, not the one on which the server is listening)
		int						_socket_fd;	// The socket which is used to communicate between client and server
		std::string				_server_addr;	// The IP address of the server
		int						_server_port;	// The port of the server FROM which the client connected (the one on which the server is listening)
		/*	
		request					_request; // All of the request/response pairs associated with this client
		response					_response;
		*/
		int						_event;
		int						_encoding;
		ssize_t					_content_length;
		ssize_t					_body_size;
		ssize_t					_chunk_size;
		int						_status;
		int						_connection;
		bool						_chunked;
		request_line_type			_request_line;
		request_headers_type		_request_headers;
		std::string				_temp;
		std::string				_current;
		Temporary					_temporary;
		std::queue<std::string>		_response;
		int						_end;
		bool						_close;

		int						_receive(std::string & content);
		int						_send(std::string content);

		/* request */
		void						_request(void);

		int						_requestLine(void);
		int						_requestMethod(std::string & source, int & dst);
		int						_requestTarget(std::string & source, std::string & dst);
		int						_requestVersion(std::string & source, std::string & dst);

		int						_requestHeaders(void);
		int						_requestHeader(std::string source, std::string & key, std::string & value);

		int						_requestBody(void);
		int						_requestBodyLength(void);
		int						_requestBodyChunked(void);
		int						_requestBodyFinished(void);
};

#endif
