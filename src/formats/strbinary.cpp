#include "strbinary.hpp"

STRBinary::STRBinary(void)
{
	this->_data.resize(0);
}

STRBinary::STRBinary(std::string str)
{
	this->_data.resize(0);
	this->append(str);
}

STRBinary::STRBinary(const char * str)
{
	this->_data.resize(0);
	this->append(str);
}

STRBinary::STRBinary(STRBinary const & other)
{ *this = other; }

STRBinary::~STRBinary()
{ }

STRBinary		&STRBinary::operator=(STRBinary const &rhs)
{
	if (this != &rhs) {
		this->_data = rhs._data;
	}

	return (*this);
}

char			STRBinary::operator[](size_t n)
{ return *(this->_data.begin() + n); }


std::size_t	STRBinary::find(std::string str)
{ return this->str().find(str); }


void		STRBinary::append(std::string str)
{
	for (size_t i = 0; i < str.size(); ++i) {
		this->_data.push_back(str[i]);
	}
}

void		STRBinary::append(const char c)
{
	this->_data.push_back(c);
}

void		STRBinary::append(std::vector<char> & other)
{
	for (size_t i = 0; i < other.size(); ++i)
		this->_data.push_back(other[i]);
}

void		STRBinary::append(const char * str, size_t size)
{
	size_t	i;

	i = 0;

	for (i = 0; i < size; ++i) {
		this->_data.push_back(str[i]);
	}
}


void		STRBinary::clear(void)
{ this->_data.clear(); }


STRBinary	STRBinary::substr(size_t from, size_t to)
{
	STRBinary	tmp;
	size_t	i;

	for (i = from; i < to; ++i)
		tmp.append(this->_data[i]);

	return tmp;
}

STRBinary		STRBinary::substr(size_t from)
{ return this->substr(from, this->_data.size()); }

std::string	STRBinary::str(void)
{ return std::string(this->_data.begin(), this->_data.end()); }

char			*STRBinary::dup(void)
{
	char		*tmp;
	size_t	i;

	i = 0;
	tmp = NULL;
	if (!(tmp = (char*)malloc(sizeof(*tmp) * (this->length() + 1))))
		return NULL;

	while (i < this->length()) {
		tmp[i] = this->_data[i];
		i++;
	}

	tmp[i] = 0;
	return tmp;
}

size_t		STRBinary::length(void)
{
	size_t	tmp = 0;

	std::vector<char>::iterator	ite = this->_data.end();
	for (std::vector<char>::iterator it = this->_data.begin(); it != ite; ++it)
		tmp++;

	return tmp;
}