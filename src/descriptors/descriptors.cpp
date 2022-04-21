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

	this->descriptors.push_back(*new_pollfd);

	delete new_pollfd;
}

std::string	Descriptors::getDescriptorType(int descriptor)
{ return this->descriptors_type[descriptor]; }

void			Descriptors::setDescriptorType(int descriptor, std::string type)
{ this->descriptors_type[descriptor] = type; }

void			Descriptors::setDescriptorEvent(int descriptor, short events)
{
	poll_type	ite = this->descriptors.end();
	for (poll_type it = this->descriptors.begin(); it != ite; ++it) {
		if (it->fd == descriptor) {
			it->events = events;
			return;
		}
	}
}

void			Descriptors::deleteDescriptor(int descriptor)
{
	poll_type	ite = this->descriptors.end();
	for (poll_type it = this->descriptors.begin(); it != ite; ++it) {
		if (it->fd == descriptor) {
			it->fd = -1;
			this->descriptors_type.erase(descriptor);
			this->descriptors.erase(it);
			return;
		}
	}
}

void			Descriptors::compressDescriptors(void) {
	size_t	i;
	size_t	j;
	size_t	descriptors_size;

	descriptors_size = this->descriptors.size();

	for (i = 0; i < descriptors_size; i++) {
		if (this->descriptors[i].fd == -1) {
			for (j = i; j < descriptors_size - 1; j++) {
				this->descriptors[j].fd = this->descriptors[j + 1].fd;
			}

			i--;
			this->descriptors.pop_back();
		}
	}
}