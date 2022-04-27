#include "tmpfile.hpp"

TmpFile::TmpFile(Descriptors *descriptors, std::string const &filename)
:
	_fd(-2),
	_size(0),
	_begin(0),
	_path(),
	_filename(filename)
{
	if ((this->_fd = uniqueFile("/tmp/webserv/", this->_path, O_CREAT | O_TRUNC | O_RDWR, S_IRWXU)) < 0)
		this->_fd = -2;


	fcntl(this->_fd, F_SETFL, O_NONBLOCK);

	this->_descriptors = descriptors;
	this->_descriptors->setDescriptor(this->_fd, POLLOUT);
	this->_descriptors->setDescriptorType(this->_fd, "file");
}

TmpFile::~TmpFile()
{ this->close(); }


/* Getters */
std::string			TmpFile::getFilename(void)
{ return (this->_filename); }

std::string			TmpFile::getPath(void)
{ return (this->_path); }

int					TmpFile::getFD(void)
{ return (this->_fd); }

size_t				TmpFile::getSize(void)
{ return (this->_size); }

Descriptors::poll_type	TmpFile::getPoll(void)
{
	Descriptors::poll_type	ite = this->_descriptors->descriptors.end();
	for (Descriptors::poll_type	it = this->_descriptors->descriptors.begin(); it != ite; ++it) {
		if (it->fd == this->_fd) {
			return it;
		}
	}

	return ite;
}

short		TmpFile::getEvents(void)
{
	Descriptors::poll_type	it = this->getPoll();
	if (it != this->_descriptors->descriptors.end()) {
		return it->revents;
	}

	return 0;
}


/* Setters */
void			TmpFile::setEvents(short events)
{
	Descriptors::poll_type	it = this->getPoll();
	if (it != this->_descriptors->descriptors.end()) {
		it->events = events;
	}
}


/* Methods */
void			TmpFile::resetCursor(void)
{ lseek(this->_fd, this->_begin, SEEK_SET); }

int			TmpFile::read(std::string & value)
{
	Descriptors::poll_type	it;
	ssize_t 				pos;
	char					buffer[BUFFER_SIZE];

	pos = 0;

	memset(buffer, 0, BUFFER_SIZE);

	if ((it = this->getPoll()) == this->_descriptors->descriptors.end())
		return -1;

	value.clear();

	if (!(it->revents & POLLIN))
		return -1;

	pos = ::read(this->_fd, buffer, BUFFER_SIZE - 1);

	value = std::string(buffer);

	return (pos > 0 && value.length() > 0);
}

int			TmpFile::clear(void)
{
	this->_begin = this->_size;
	this->_size = 0;
	return (1);
}

int			TmpFile::read(STRBinary & value)
{
	Descriptors::poll_type	it;
	std::vector<char>		packet(5000);
	ssize_t				pos;

	pos = 0;

	if ((it = this->getPoll()) == this->_descriptors->descriptors.end()) {
		return -1;
	}

	value.clear();

	if (!(it->revents & POLLIN))
		return -1;

	pos = ::read(this->_fd, packet.data(), packet.size());

	if (static_cast<int>(packet.size()) > pos)
		packet.resize(pos);

	value = packet;

	return (pos > 0 && value.length() > 0);
}

int			TmpFile::write(std::string const & value)
{
	this->_size += value.length();
	return ::write(this->_fd, value.c_str(), value.length());
}

int			TmpFile::write(STRBinary value)
{
	this->_size += value.length();
	return ::write(this->_fd, value.c_str(), value.length());
}

void			TmpFile::close(void) {
	if (this->_fd < 0)
		return ;
	::close(this->_fd);
	remove(secureAddress("/tmp/webserv/", this->_path).c_str());
	this->_descriptors->deleteDescriptor(this->_fd);
}