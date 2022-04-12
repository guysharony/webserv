#ifndef DESCRIPTORS_HPP
# define DESCRIPTORS_HPP

# include <iostream>
# include <string>
# include <map>
# include <vector>
# include "../sockets/sockets.hpp"

class Descriptors {

	public:
		Descriptors(void);
		virtual ~Descriptors();

		typedef std::vector<pollfd>		polls_type;
		typedef polls_type::iterator		poll_type;

		Sockets						sockets;

		std::string 					getDescriptorType(int fd);

		void							setDescriptorType(int fd, std::string type);

		void							deleteDescriptor(int fd);
		void							serversInitialize(void);

	private:
		std::map<int, std::string>		_descriptors;

};

#endif