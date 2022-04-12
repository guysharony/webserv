#include "descriptors.hpp"

Descriptors::Descriptors()
{ }

Descriptors::~Descriptors()
{ }

std::string	Descriptors::getDescriptorType(int fd)
{ return this->_descriptors[fd]; }

void			Descriptors::setDescriptorType(int fd, std::string type)
{ this->_descriptors[fd] = type; }

void			Descriptors::deleteDescriptor(int fd)
{ this->_descriptors.erase(fd); }

void			Descriptors::serversInitialize(void) {
	int		fd;

	for (Sockets::socketsListenerType::iterator	it = this->sockets.sockets.begin(); it != this->sockets.sockets.end(); it++) {
		if ((fd = this->sockets.initialize(it)) > 0) {
			this->setDescriptorType(fd, "server");
		}
	}
}