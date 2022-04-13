#ifndef DESCRIPTORS_HPP
# define DESCRIPTORS_HPP

# include <iostream>		// std::cout
# include <sys/types.h>		// socket - included for portability
# include <sys/socket.h> 	// socket, AF_LOCAL, SOCK_STREAM, inet_addr
# include <cstring>			// memset
# include <string>			// string
# include <netinet/in.h> 	// sockaddr_in, inet_addr
# include <arpa/inet.h>		// htons, inet_addr
# include <unistd.h>		// close
# include <vector>			// vector
# include <sys/poll.h>		// poll
# include <sys/ioctl.h>
# include <set>			// set
# include <map>			// map
# include <queue>			// queue
# include "../../include/constants.hpp"

class Descriptors {

	public:
		Descriptors(void);
		~Descriptors();

		typedef std::vector<pollfd>		polls_type;
		typedef polls_type::iterator		poll_type;

		std::map<int, std::string>		descriptors_type;
		polls_type					descriptors;

		/* Getters */
		std::string 					getDescriptorType(int descriptor);

		/* Setters */
		void							setDescriptor(int descriptor, short events);
		void							setDescriptorType(int descriptor, std::string type);

		/* Methods */
		void							deleteDescriptor(int descriptor);

};

#endif