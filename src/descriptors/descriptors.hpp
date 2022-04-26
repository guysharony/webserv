#ifndef DESCRIPTORS_HPP
# define DESCRIPTORS_HPP

# include <iostream>
# include <sys/types.h>
# include <sys/socket.h>
# include <cstring>
# include <string>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <unistd.h>
# include <vector>
# include <sys/poll.h>
# include <sys/ioctl.h>
# include <set>
# include <map>
# include <queue>
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
		void							setDescriptorEvent(int descriptor, short events);

		/* Methods */
		void							deleteDescriptor(int descriptor);
		void							compressDescriptors(void);

};

#endif