#include "descriptors.hpp"

Descriptors::Descriptors()
{ }

Descriptors::~Descriptors()
{ }

void			Descriptors::setDescriptor(int descriptor, short events)
{
	pollfd* new_pollfd = new pollfd;
	new_pollfd->fd = descriptor;
	new_pollfd->events = events;

	std::cout << "CREATE [" << descriptor << "]" << std::endl;

	this->descriptors.push_back(*new_pollfd);

	poll_type ite = this->descriptors.end();
	for (poll_type it = this->descriptors.begin(); it != ite; ++it) {
		std::cout << RED << "FILE [" << it->fd << "]" << RESET << std::endl;
	}

	delete new_pollfd;
}

std::string	Descriptors::getDescriptorType(int descriptor)
{ return this->descriptors_type[descriptor]; }

void			Descriptors::setDescriptorType(int descriptor, std::string type)
{ this->descriptors_type[descriptor] = type; }

void			Descriptors::deleteDescriptor(int descriptor)
{
	poll_type	ite = this->descriptors.end();
	for (poll_type it = this->descriptors.begin(); it != ite; ++it) {
		if (it->fd == descriptor) {
			this->descriptors_type.erase(descriptor);
			this->descriptors.erase(it);
			return;
		}
	}
}