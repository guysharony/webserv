#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <string>
# include <map>
# include <queue>

# include "request.hpp"
// # include "response.hpp"

class Client
{
	public:
	typedef request										request_type;
	typedef std::string									response_type; // should be Response class
	typedef std::pair < request_type, response_type >	pair_type;

	// Using a queue of pairs allows us to handle several requests from the same client while maintaining that each request has a one-to-one relationship with a response.
	typedef std::queue< pair_type >						request_container;

	Client(void);
	Client(Client const & src);
	~Client(void);
	Client &operator=(Client const & rhs);

	std::string const	&getIpAddr(void);
	int					getPort(void);
	int					getSocketFd(void);
	void				setIpAddr(std::string const &ip_addr);
	void				setPort(int port);
	void				setSocketFd(int socket_fd);

	void				addRequest(std::string const & raw_request);		// Add a new request that has just been received by the server
	void				updateRequest(std::string const & raw_request);		// Update the last request if more data is received by the server
	void				deleteRequest();									// Delete a request after response has been sent
	pair_type			&getRequest();										// Get next request from queue

	private:
	std::string			_ip_addr;	// The IP address of the client
	int					_port;		// The port of the server which is connected to the client (the one created by accept, not the one on which the server is listening)
	int					_socket_fd;	// The socket which is used to communicate between client and server
	request_container	_requests;	// All of the request/response pairs associated with this client
};


#endif
