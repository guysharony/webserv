#include "descriptors.hpp"

Descriptors::Descriptors()
: _compress_array(false)
{ }

Descriptors::~Descriptors()
{ }

void			Descriptors::setDescriptor(int descriptor, short events)
{
	pollfd* new_pollfd = new pollfd;
	new_pollfd->fd = descriptor;
	new_pollfd->events = events;

	// std::cout << "CREATE [" << descriptor << "]" << std::endl;

	this->descriptors.push_back(*new_pollfd);

	/*
	poll_type ite = this->descriptors.end();
	for (poll_type it = this->descriptors.begin(); it != ite; ++it) {
		std::cout << RED << "FILE [" << it->fd << "]" << RESET << std::endl;
	}
	*/

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
			this->_compress_array = true;
			close(it->fd);
			this->descriptors_type.erase(it->fd);
			it->fd = -1;
			return;
		}
	}
}

void			Descriptors::compressDescriptors(void) {
	size_t	i;
	size_t	j;
	size_t	descriptors_size;

	if (this->_compress_array) {
		this->_compress_array = false;
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
}