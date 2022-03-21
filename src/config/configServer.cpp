#include "configServer.hpp"

ConfigServer::ConfigServer(void)
{ }

ConfigServer::ConfigServer(ConfigServer const & src)
{ *this = src; }

ConfigServer::~ConfigServer()
{ }

void			ConfigServer::parseServer(void) {
	configuration_struct	config;
	std::string			word;

	this->compressed_configuration_file_iterator = this->compressed_configuration_file.begin();

	while ((word = this->_extractWord()) != "") {
		if (word != "server" || (this->_extractWord() != "{")) {
			Message::error("Configuration file corrupted.");
		}

		config.auto_index = -1;

		while ((word = this->_extractWord()) != "}") {
			if (word == "listen") {
				this->_parseListen(config);
			} else if (word == "server_name") {
				this->_parseServerName(config);
			} else if (word == "root") {
				this->_parseRoot(config);
			} else if (word == "location") {
				this->parseLocation(config);
			} else if (word == "index") {
				this->_parseIndex(config);
			} else if (word == "auto_index") {
				this->_parseAutoIndex(config);
			} else if (word == "client_max_body_size") {
				this->_parseClientMaxBodySize(config);
			} else if (word == "redirect") {
				this->_parseRedirect(config);
			} else if (word == "error_page") {
				this->_parseErrorPage(config);
			} else if (word == "cgi_path") {
				this->_parseCGIPath(config);
			} else if (word == "cgi_extentions") {
				this->_parseCGIExtentions(config);
			} else {
				Message::error("Unknown directive: " + word);
			}
		}
	}

	this->configuration.push_back(config);
}

std::string 	ConfigServer::_extractWord(void) {
	std::string	res;

	while (this->compressed_configuration_file_iterator < this->compressed_configuration_file.end() && *this->compressed_configuration_file_iterator == ' ')
		this->compressed_configuration_file_iterator++;

	while (this->compressed_configuration_file_iterator < this->compressed_configuration_file.end() && *this->compressed_configuration_file_iterator != ' ') {
		res.push_back(*this->compressed_configuration_file_iterator);
		this->compressed_configuration_file_iterator++;
	}

	return (res);
}

void			ConfigServer::_parseListen(configuration_struct &config) {
	if (config.port != "")
		Message::error("'port' already assigned.");

	std::string			authority = this->_extractWord();
	std::vector<std::string>	params = split(authority, ":");

	if (params.size() == 2) {
		config.host = params[0];
		config.port = params[1];
	} else if (params.size() == 1)
		config.port = params[0];

	if (this->_extractWord() != ";")
		Message::error("'port' already assigned.");
}

void			ConfigServer::_parseServerName(configuration_struct &config) {
	if (config.server_name != "")
		Message::error("'server_name' already assigned.");

	config.server_name = this->_extractWord();

	if (this->_extractWord() != ";")
		Message::error("'server_name' already assigned.");
}

void			ConfigServer::_parseRoot(configuration_struct &config) {
	if (config.root != "")
		Message::error("'root' already assigned.");

	config.root = this->_extractWord();

	if (this->_extractWord() != ";")
		Message::error("'root' already assigned.");
}

void			ConfigServer::_parseIndex(configuration_struct &config) {
	std::string word;

	if (config.index.front() != "")
		Message::error("'index' already assigned.");

	while ((word = this->_extractWord()) != "}" && word != ";")
		config.index.push_back(word);

	if (word == "}")
		Message::error("';' is expected.");
}

void			ConfigServer::_parseAutoIndex(configuration_struct &config) {
	std::string	auto_index;

	auto_index = this->_extractWord();

	if (auto_index == "on")
		config.auto_index = 1;
	else if (auto_index == "off")
		config.auto_index = 0;
	else
		Message::error("'auto_index' cannot be set to '" + auto_index + "'.");

	if (this->_extractWord() != ";")
		Message::error("';' is expected.");
}

void			ConfigServer::_parseClientMaxBodySize(configuration_struct &config) {
	if (config.client_max_body_size != "")
		Message::error("'client_max_body_size' already assigned.");

	config.client_max_body_size = this->_extractWord();

	if (this->_extractWord() != ";")
		Message::error("'client_max_body_size' already assigned.");
}

void			ConfigServer::_parseRedirect(configuration_struct &config) {
	if (config.redirect != "")
		Message::error("'redirect' already assigned.");

	config.redirect = this->_extractWord();

	if (this->_extractWord() != ";")
		Message::error("'redirect' already assigned.");
}

void			ConfigServer::_parseErrorPage(configuration_struct &config) {
	int			argument;
	std::string	parameter;
	std::string	error_code;
	std::string	error_page;

	argument = 0;

	while ((parameter = this->_extractWord()) != "}" && parameter != ";")
	{
		if (argument >= 2)
			Message::error("Too many arguments for error page.");
		if (argument == 0)
			error_code = parameter;
		if (argument == 1)
			error_page = parameter;

		argument++;
	}

	if (parameter == "}")
		Message::error("';' is expacted.");

	if (config.error_page.find(error_code) != config.error_page.end())
		Message::error("Error '" + error_code + "' has already been declared.");

	config.error_page[error_code] = error_page;
}

void		ConfigServer::_parseCGIPath(configuration_struct &config) {
	if (config.cgi_path != "")
		Message::error("'cgi_path' already assigned.");

	config.cgi_path = this->_extractWord();

	if (this->_extractWord() != ";")
		Message::error("'cgi_path' already assigned.");
}

void		ConfigServer::_parseCGIExtentions(configuration_struct &config) {
	std::string	parameter;

	if (config.cgi_extentions.front() != "")
		Message::error("cgi_extentions already assigned.");

	while ((parameter = this->_extractWord()) != "}" && parameter != ";")
		config.cgi_extentions.push_back(parameter);

	if (parameter == "}")
		Message::error("';' is expected.");
}