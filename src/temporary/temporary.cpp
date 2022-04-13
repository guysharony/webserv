#include "temporary.hpp"

Temporary::Temporary(void)
{ srand(time(NULL)); }

Temporary::Temporary(Descriptors *descriptors)
: _descriptors(descriptors)
{
	srand(time(NULL));
}

Temporary::~Temporary()
{
	tmpfile_type	ite = this->_tmpfiles.end();
	for (tmpfile_type it = this->_tmpfiles.begin(); it != ite; ++it)
		delete *it;

	this->_tmpfiles.clear();
}

bool		Temporary::create(std::string const &filename)
{
	tmpfile_type	ite = this->_tmpfiles.end();
	for (tmpfile_type it = this->_tmpfiles.begin(); it != ite; ++it) {
		if ((*it)->getFilename() == filename)
			return true;
	}

	this->_tmpfiles.push_back(new TmpFile(this->_descriptors, filename));
	return false;
}

bool		Temporary::clear(std::string const &filename)
{
	tmpfile_type	ite = this->_tmpfiles.end();
	for (tmpfile_type it = this->_tmpfiles.begin(); it != ite; ++it) {
		if ((*it)->getFilename() == filename)
			return (*it)->clear();
	}

	return false;
}

short	Temporary::getEvents(std::string const &filename)
{
	tmpfile_type	ite = this->_tmpfiles.end();
	for (tmpfile_type it = this->_tmpfiles.begin(); it != ite; ++it) {
		if ((*it)->getFilename() == filename) {
			return (*it)->getEvents();
		}
	}

	return 0;
}

bool		Temporary::setEvents(std::string const &filename, short event)
{
	tmpfile_type	ite = this->_tmpfiles.end();
	for (tmpfile_type it = this->_tmpfiles.begin(); it != ite; ++it) {
		if ((*it)->getFilename() == filename) {
			(*it)->setEvents(event);
			return true;
		}
	}

	return false;
}

int		Temporary::read(std::string const &filename, std::string &dest)
{
	tmpfile_type	ite = this->_tmpfiles.end();
	for (tmpfile_type it = this->_tmpfiles.begin(); it != ite; ++it) {
		if ((*it)->getFilename() == filename) {
			return (*it)->read(dest);
		}
	}

	return -1;
}

int		Temporary::append(std::string const &filename, std::string const & source)
{
	tmpfile_type	ite = this->_tmpfiles.end();
	for (tmpfile_type it = this->_tmpfiles.begin(); it != ite; ++it) {
		if ((*it)->getFilename() == filename) {
			return (*it)->write(source);
		}
	}

	return -1;
}

int		Temporary::display(std::string const &filename)
{
	tmpfile_type	ite = this->_tmpfiles.end();
	for (tmpfile_type it = this->_tmpfiles.begin(); it != ite; ++it) {
		if ((*it)->getFilename() == filename) {
			return (*it)->display();
		}
	}

	return -1;
}

bool		Temporary::close(std::string const &filename)
{
	tmpfile_type	ite = this->_tmpfiles.end();
	for (tmpfile_type it = this->_tmpfiles.begin(); it != ite; ++it) {
		if ((*it)->getFilename() == filename) {
			delete *it;
			it = this->_tmpfiles.erase(it);
			return true;
		}
	}

	return false;
}

bool		Temporary::resetCursor(std::string const &filename)
{
	tmpfile_type	ite = this->_tmpfiles.end();
	for (tmpfile_type it = this->_tmpfiles.begin(); it != ite; ++it) {
		if ((*it)->getFilename() == filename) {
			(*it)->resetCursor();
			return true;
		}
	}

	return false;
}

void		Temporary::setDescriptors(Descriptors *descriptors)
{ this->_descriptors = descriptors; }