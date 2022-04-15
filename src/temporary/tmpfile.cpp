#include "tmpfile.hpp"

TmpFile::TmpFile(Descriptors *descriptors, std::string const &filename)
:
	_fd(-1),
	_path(),
	_filename(filename)
{
	mkdir("/tmp/webserv/", 0777);

	this->_path = this->_generate_filepath();

	this->_fd = open(this->_path.c_str(), O_CREAT | O_TRUNC | O_RDWR, S_IRWXU);
	if (this->_fd < 0)
		Message::error("Failed in creating file.");

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
{ lseek(this->_fd, 0, SEEK_SET); }

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

	pos = ::read(this->_fd, buffer, BUFFER_SIZE - 1);

	value = std::string(buffer);

	return (pos > 0 && value.length() > 0);
}

int			TmpFile::write(std::string const & value)
{ return ::write(this->_fd, value.c_str(), value.length()); }

std::string	TmpFile::_generate_filepath(void) {
	std::string	name;

	name = "/tmp/webserv/" + intToHex(rand() % 9999999 + 1000000) + "_" + intToHex(rand() % 9999999 + 1000000);

	return exists(name) ? this->_generate_filepath() : name;
}

void			TmpFile::close(void) {
	::close(this->_fd);
	remove(this->_path.c_str());
	this->_descriptors->deleteDescriptor(this->_fd);
}

int			TmpFile::display(void)
{
	char			letter;
	std::ifstream 	file(this->_path.c_str(), std::ifstream::binary);

	if (!file) {
		std::cout << "Can't open file." << std::endl;
		return (0);
	}

	while (!file.eof()) {
		if (!file.get(letter)) letter = 0;

		std::cout << letter;
	}

	file.close();

	return (1);
}