#include "config.hpp"

Config::Config(void)
{ }

Config::Config(Config const & src) : ConfigServer()
{ *this = src; }

Config::~Config()
{ }

int			Config::load(std::string filename)
{
	int result = this->_parseConfiguration(filename);

	#ifdef DEBUG
		for (configuration_type it = this->configuration.begin(); it != this->configuration.end(); it++)
		{
			std::cout << "[ server name => (" << it->server_name << ") ]" << std::endl;

			if (it->listen.size()) {
				std::cout << "[ listen: ]" << std::endl;
				for (listen_type::iterator it2 = it->listen.begin(); it2 != it->listen.end(); it2++) {
					std::cout << "[    (" << it2->first << ") => ";
					for (ports_type::iterator it3 = it2->second.begin(); it3 != it2->second.end(); it3++) {
						std::cout << *it3 << ", ";
					}
					std::cout << "]" << std::endl;
				}
			}

			std::cout << "[ root => (" << it->root << ") ]" << std::endl;

			if (it->redirect.compare(""))
				std::cout << "[ redirect => (" << it->redirect << ") ]" << std::endl;
			std::cout << "[ cgi_path => (" << it->cgi_path << ") ]" << std::endl;

			std::cout << "[ cgi_extentions: => (";
			for (std::vector<std::string>::iterator it2 = it->cgi_extentions.begin(); it2 != it->cgi_extentions.end(); it2++)
				std::cout << *it2 << ", ";
			std::cout << ") ]" << std::endl;

			std::cout << "[ auto_index => (" << it->auto_index << ") ]" << std::endl;

			if (it->index.size()) {
				std::cout << "[ index: ]" << std::endl;
				for (std::vector<std::string>::iterator it2 = it->index.begin(); it2 != it->index.end(); it2++) {
					std::cout << "[    " << *it2 << "]" << std::endl;
				}
			}

			if (it->error_page.size()) {
				std::cout << "[ error_page: ]" << std::endl;
				for (error_pages_type::iterator it2 = it->error_page.begin(); it2 != it->error_page.end(); it2++) {
					std::cout << "[    (" << it2->first << " => " << it2->second << ") ]" << std::endl;
				}
			}
		}
	#endif

	return result;
}

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