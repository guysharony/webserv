#include "tmpfile.hpp"

TmpFile::TmpFile(std::string const &filename)
:
	_fd(-1),
	_path(),
	_filename(filename)
{
	mkdir("/tmp/webserv/", 0777);

	this->_path = this->_generate_filepath();

	this->_fd = open(this->_path.c_str(), O_CREAT | O_TRUNC | O_RDWR, S_IRWXU);

	fcntl(this->_fd, F_SETFL, O_NONBLOCK);
}

TmpFile::~TmpFile()
{
	close(this->_fd);
	unlink(this->_path.c_str());
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

std::string	TmpFile::getFilename(void)
{ return (this->_filename); }

std::string	TmpFile::getPath(void)
{ return (this->_path); }


void			TmpFile::resetCursor(void)
{ lseek(this->_fd, 0, SEEK_SET); }

int			TmpFile::read(std::string & value)
{
	char			buffer[BUFFER_SIZE] = { 0 };

	value.clear();

	ssize_t pos = ::read(this->_fd, buffer, BUFFER_SIZE - 1);

	value = std::string(buffer);

	return (pos > 0 && value.length() > 0);
}

int			TmpFile::write(std::string const & value)
{ return ::write(this->_fd, value.c_str(), value.length()); }

std::string	TmpFile::_generate_filepath(void) {
	std::string	name;

	name = "/tmp/webserv/" + intToHex(rand() % 999999 + 1) + "_" + intToHex(rand() % 999999 + 1);

	return exists(name) ? this->_generate_filepath() : name;
}