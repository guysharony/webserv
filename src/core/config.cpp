#include "config.hpp"

Config::Config(void)
{ signal(SIGPIPE, SIG_IGN); }

Config::Config(Config const & src)
{ *this = src; }

Config::~Config()
{ }

int			Config::load(std::string filename)
{
	this->_filename = filename;
	int result = this->_parseConfiguration();

	#ifdef DEBUG
		for (config_type it = conf.begin(); it != conf.end(); it++)
		{
			std::cout << "[ server name => (" << it->server_name << ") ]" << std::endl;
			std::cout << "[ host => (" << it->host << ") ]" << std::endl;
			std::cout << "[ port => (" << it->port << ") ]" << std::endl;
			std::cout << "[ root => (" << it->root << ") ]" << std::endl;

			if (it->redirect.compare(""))
				std::cout << "[ redirect => (" << it->redirect << ") ]" << std::endl;
			std::cout << "[ cgi_path => (" << it->cgi_path << ") ]" << std::endl;

			std::cout << "[ cgi_extentions: => (";
			for (std::list<std::string>::iterator it2 = it->cgi_extentions.begin(); it2 != it->cgi_extentions.end(); it2++)
				std::cout << *it2 << ", ";
			std::cout << ") ]" << std::endl;

			std::cout << "[ auto_index => (" << it->auto_index << ") ]" << std::endl;

			if (it->index.size()) {
				std::cout << "[ index: ]" << std::endl;
				for (std::list<std::string>::iterator it2 = it->index.begin(); it2 != it->index.end(); it2++) {
					std::cout << "[    " << *it2 << "]" << std::endl;
				}
			}

			if (it->error_page.size()) {
				std::cout << "[ error_page: ]" << std::endl;
				for (std::map<std::string, std::string>::iterator it2 = it->error_page.begin(); it2 != it->error_page.end(); it2++) {
					std::cout << "[    (" << it2->first << " => " << it2->second << ") ]" << std::endl;
				}
			}
		}
	#endif

	return result;
}

int			Config::load(char *filename)
{ return this->load(std::string(filename)); }

int			Config::_parseConfiguration(void) {
	int					file_descriptor;
	int					result;
	int					brakets;
	std::string			line;
	std::string			parsed_file;

	if ((file_descriptor = open(this->_filename.c_str(), O_RDONLY)) < 0)
		Message::error("Configuration file corrupted.");

	brakets = 0;

	while ((result = readFileLines(file_descriptor, line)) != -1) {
		this->_parseConfigurationLine(brakets, line, parsed_file);

		if (!result)
			break;
	}

	if (brakets > 0) {
		Message::error("'}' is expected.");
	}

	close(file_descriptor);

	this->_parseServer(parsed_file);

	return 1;
}

void			Config::_parseConfigurationLine(int &brakets, std::string const line, std::string &result) {
	int		i;

	i = 0;
	while (line[i]) {
		if (this->_isLimiter(brakets, line[i])) {
			this->_addSpaceAfterLimiter(i, result);
			result.push_back(line[i++]);
		} else if (this->_isSpace(line[i])) {
			while (line[i] && this->_isSpace(line[i]))
				i++;
			result.push_back(' ');
		} else if (this->_isComment(line[i])) {
			return;
		} else {
			result.push_back(line[i++]);
		}
	}
}

void			Config::_parseServer(std::string const parsed_file) {
	std::string::iterator	parsed_file_iterator = parsed_file.begin();
	std::string			word;

	configuration_struct	config;

	while ((word = this->_extractWord(parsed_file, parsed_file_iterator)) != "") {
		if (tmp != "Server" || (this->_extractWord(parsed_file, parsed_file_iterator) != "{")) {
			Message::error("Configuration file corrupted.");
		}

		config.auto_index = -1;
		
		while ((word = this->_extractWord(parsed_file, parsed_file_iterator)) != '}') {
			if (word == "listen") {
				if (config.port != "")
					Message::error("'port' already assigned.");

				std::string			authority = this->_extractWord(parsed_file, parsed_file_iterator);
				std::vector<std::string>	params = split(authority, ":");

				if (params.size() == 2) {
					config.host = params[0];
					config.port = params[1];
				} else if (params.size() == 1)
					config.port = params[0];

				if (this->_extractWord(parsed_file, parsed_file_iterator) != ";")
					Message::error("'port' already assigned.");
			} else if (word == "location") {
				// PARSE LOCATION
			} else if (word == "server_name") {
				if (config.server_name != "")
					Message::error("'server_name' already assigned.");

				config.server_name = this->_extractWord(parsed_file, parsed_file_iterator);

				if (this->_extractWord(parsed_file, parsed_file_iterator) != ";")
					Message::error("'server_name' already assigned.");
			} else if (word == "root") {
				if (config.server_name != "")
					Message::error("'root' already assigned.");

				config.root = this->_extractWord(parsed_file, parsed_file_iterator);

				if (this->_extractWord(parsed_file, parsed_file_iterator) != ";")
					Message::error("'root' already assigned.");
			} else if (word == "index") {
				if (config.index.front() != "")
					Message::error("'index' already assigned.");

				while ((word = this->_extractWord(parsed_file, parsed_file_iterator)) != "}" && word != ";")
					config.index.push_back(word);

				if (word == "}")
					Message::error("';' is expected.");
			} else if (word == "auto_index") {
				word = this->_extractWord(parsed_file, parsed_file_iterator);

				if (word == "on")
					config.auto_index = 1;
				else if (tmp == "off")
					config.auto_index = 0;
				else
					Message::error("'auto_index' cannot be set to '" + word + "'.");

				if (this->_extractWord(str, it) != ";")
					Message::error("';' is expected.");
			} else if (word == "client_max_body_size") {
				if (config.client_max_body_size != "")
					Message::error("'client_max_body_size' already assigned.");

				config.client_max_body_size = this->_extractWord(parsed_file, parsed_file_iterator);

				if (this->_extractWord(parsed_file, parsed_file_iterator) != ";")
					Message::error("'client_max_body_size' already assigned.");
			} else if (word == "redirect") {
				if (config.redirect != "")
					Message::error("'redirect' already assigned.");

				config.redirect = this->_extractWord(str, it);

				if (this->_extractWord(parsed_file, parsed_file_iterator) != ";")
					Message::error("'redirect' already assigned.");
			} else if (word == "error") {
				int			argument;
				std::string	error_code;
				std::string	error_page;

				argument = 0;

				while ((word = this->_extractWord(parsed_file, parsed_file_iterator)) != "}" && word != ";")
				{
					if (argument >= 2)
						Message::error("Too many arguments for error page.");
					if (argument == 0)
						error_code = word;
					if (argument == 1)
						error_page = word;

					argument++;
				}

				if (word == "}")
					Message::error("';' is expacted.");

				if (config.error_page.find(error) != config.error_page.end())
					Message::error("Error '" + error + "' has already been declared.");

				config.error_page[error] = path;
			} else if (word == "cgi_path") {
				if (config.cgi_path != "")
					Message::error("'cgi_path' already assigned.");

				config.cgi_path = this->_extractWord(parsed_file, parsed_file_iterator);

				if (this->_extractWord(parsed_file, parsed_file_iterator) != ";")
					Message::error("'cgi_path' already assigned.");
			} else if (word == "cgi_extentions") {
				if (config.cgi_extentions.front() != "")
					Message::error("cgi_extentions already assigned.");

				while ((word = this->_extractWord(parsed_file, parsed_file_iterator)) != "}" && word != ";")
					config.cgi_extentions.push_back(word);

				if (word == "}")
					Message::error("';' is expected.");
			} else {
				Message::error("Unknown directive: " + word);
			}
		}
	}

	this->configuration.push_back(config);
}

std::string 	Config::_extractWord(std::string &str, std::string::iterator &it) {
	std::string res;

	while (it < str.end() && *it == ' ')
		it++;

	while (it < str.end() && *it != ' ') {
		res.push_back(*it);
		it++;
	}

	return (res);
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

void			Config::_addSpaceAfterLimiter(int i, std::string &result) {
	bool		previous_line_space = !i && result.length() > 0 && !this->_isSpace(result[result.length() - 1]);
	
	if (previous_line_space) {
		result.push_back(' ');
	}
}