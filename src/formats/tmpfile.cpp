#include "tmpfile.hpp"

TmpFile::TmpFile(void)
{ }

TmpFile::TmpFile(TmpFile const & src)
{ *this = src; }

TmpFile::~TmpFile()
{ }

TmpFile		&TmpFile::operator=(const TmpFile &src)
{
	this->_filename = src._filename;
	this->_position = src._position;
	return (*this);
}

int			TmpFile::display(void)
{
	char			letter;
	std::ifstream 	file(this->_filename.c_str(), std::ifstream::binary);

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

ssize_t	TmpFile::findline(std::string value) {
	std::string	line;
	std::ifstream 	file(this->_filename.c_str(), std::ifstream::binary);

	if (!file) {
		Message::error("Can't open file.");
		return -1;
	}

	for (unsigned int i = 0; getline(file, line); i++) {
		if (line.compare(value + "\r") == 0) {
			file.close();
			return i;
		}
	}

	return -1;
}

void			TmpFile::position(ssize_t value)
{
	this->_position = value < 0 ? this->size() : value;
}

size_t		TmpFile::getPosition(void)
{
	return (this->_position);
}

std::string	TmpFile::getFilename(void)
{
	return (this->_filename);
}

size_t		TmpFile::size(void)
{
	std::ifstream file(this->_filename.c_str(), std::ifstream::binary);

	if (!file) {
		std::cout << "Can't open file." << std::endl;
		return (0);
	}

	file.seekg(0, file.end);
	return (file.tellg());
}

int			TmpFile::read(std::string & value)
{
	value.clear();
	char			buffer[1001] = { 0 };

	std::ifstream 	file(this->_filename.c_str(), std::ifstream::binary);

	if (!file) {
		std::cout << "Can't open file." << std::endl;
		return (0);
	}

	file.seekg(this->_position);
	file.read(buffer, 999);

	ssize_t pos = file.tellg();

	this->position(pos);

	value = std::string(buffer);

	return (pos > 0 && value.length() > 0);
}

int			TmpFile::create(std::string filename)
{
	this->_filename = filename;

	std::ofstream	ofs(this->_filename.c_str());

	if (!ofs) {
		std::cout << "Can't open file." << std::endl;
		return (0);
	}

	this->position(0);

	ofs.close();
	return (1);
}

int			TmpFile::clear(void)
{
	std::ofstream	file(this->_filename.c_str());

	if (!file) {
		std::cout << "Can't open file." << std::endl;
		return (0);
	}

	file.clear();
	file.close();

	return (1);
}

int			TmpFile::close(void)
{
	std::ofstream	file(this->_filename.c_str());

	if (!file) {
		std::cout << "Can't open file." << std::endl;
		return (0);
	}

	file.close();
	remove(this->_filename.c_str());

	return (1);
}

int			TmpFile::write(std::string value)
{
	std::ofstream	file(this->_filename.c_str(), std::ios::app);

	if (!file) {
		std::cout << "Can't open file." << std::endl;
		return (0);
	}

	file.seekp(this->_position);
	file << value;

	this->_position += value.length();

	file.close();

	return (1);
}