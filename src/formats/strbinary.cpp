#include "strbinary.hpp"

STRBinary::STRBinary(void)
{ }

STRBinary::STRBinary(std::string str)
{ this->append(str); }

STRBinary::STRBinary(const char * str)
{ this->append(str); }

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
{
	size_t	i;
	size_t	j;

	for (i = 0; i < this->_data.size(); ++i) {
		for (j = 0; j < str.size(); ++j) {
			if (this->_data[i + j] != str[j])
				break;
		}

		if (j == str.size()) {
			std::cout << "[" << str << "](" << i << ")" << std::endl;
			return (i);
		}
	}

	return (std::string::npos);
}


void		STRBinary::append(std::string str)
{
	for (size_t i = 0; i < str.size(); ++i) {
		this->_data.push_back(str[i]);
	}
}

void		STRBinary::append(const char c)
{
	this->_data.resize(this->_data.size() + 1);
	this->_data.push_back(c);
}

void		STRBinary::append(std::vector<char> & other)
{
	this->_data.resize(this->_data.size() + other.size());

	for (size_t i = 0; i < other.size(); ++i)
		this->_data.push_back(other[i]);
}

void		STRBinary::append(const char * str, size_t size)
{
	size_t	i;

	i = 0;
	
	this->_data.resize(this->_data.size() + size);

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
{
	std::string	tmp;

	for (size_t i = 0; i < this->_data.size(); ++i)
		tmp += this->_data[i];

	return tmp;
}

size_t		STRBinary::length(void)
{ return (this->_data.size()); }