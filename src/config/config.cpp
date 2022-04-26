#include "config.hpp"

Config::Config(void)
{ }

Config::Config(Config const & src) : ConfigServer()
{ *this = src; }

Config::~Config()
{ }

int			Config::load(std::string filename)
{ return this->_parseConfiguration(filename); }

int			Config::load(char *filename)
{
	std::string tmp(filename);
	return this->load(tmp);
}

int			Config::_parseConfiguration(std::string filename) {
	int				brakets = 0;
	std::string		line;
	std::ifstream		file(filename.c_str(), std::ios::in);

	if (file) {
		while (getline(file, line, '\n')) {
			this->_parseConfigurationLine(brakets, line.c_str());
		}
		file.close();
	} else {
		Message::error("Configuration file corrupted.");
	}

	brakets = 0;

	if (brakets > 0) {
		Message::error("'}' is expected.");
	}

	file.close();

	this->parseServer();

	return 1;
}

void			Config::_parseConfigurationLine(int &brakets, char const *line) {
	int		i;

	i = 0;
	while (line[i]) {
		if (this->_isLimiter(brakets, line[i])) {
			this->compressed_configuration_file.push_back(' ');
			this->compressed_configuration_file.push_back(line[i++]);
			this->compressed_configuration_file.push_back(' ');
		} else if (this->_isSpace(line[i])) {
			while (line[i] && this->_isSpace(line[i]))
				i++;
			this->compressed_configuration_file.push_back(' ');
		} else if (this->_isComment(line[i])) {
			return;
		} else {
			this->compressed_configuration_file.push_back(line[i++]);
		}
	}
}

bool			Config::_isLimiter(int &brackets, char character) {
	if (character == '{') {
		brackets++;
		return true;
	}

	if (character == '}') {
		if (brackets <= 0)
			Message::error("'{' is expected.");

		brackets--;
		return true;
	}

	return character == ';';
}

bool			Config::_isSpace(char character) {
	return character == ' ' || character == '\t';
}

bool			Config::_isComment(char character) {
	return character == '#';
}