#include "configServer.hpp"

ConfigServer::ConfigServer(void)
{ }

ConfigServer::ConfigServer(ConfigServer const & src) : ConfigLocation()
{ *this = src; }

ConfigServer::~ConfigServer()
{ }

void			ConfigServer::parseServer(void) {
	std::string			word;

	this->compressed_configuration_file_iterator = this->compressed_configuration_file.begin();

	while (!(word = this->_extractWord()).empty()) {
		configuration_struct	config;

		if (word != "server" || (this->_extractWord() != "{")) {
			Message::error("Configuration file corrupted.");
		}

		config.auto_index = -1;
		config.client_max_body_size = -1;

		#ifdef WWW
			config.root = WWW;
		#endif

		while ((word = this->_extractWord()) != "}") {
			if (word == "listen") {
				this->_parseListen(config);
			} else if (word == "server_names") {
				this->_parseServerNames(config);
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

		this->_adjustConfiguration(config);

		this->configuration.push_back(config);
	}
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
	std::string			host;
	std::string			port;

	std::string			authority = this->_extractWord();
	std::vector<std::string>	params = split(authority, ":");

	if (params.size() != 2 && params.size() != 1)
		Message::error("'listen' is not valid.");

	host = params.size() == 2 ? params[0] : "0.0.0.0";
	port = params.size() == 2 ? params[1] : params[0];

	for (listen_type::iterator it2 = config.listen.begin(); it2 != config.listen.end(); it2++) {
		for (ports_type::iterator it3 = it2->second.begin(); it3 != it2->second.end(); it3++) {
			if (it2->first == host && (*it3) == port)
				Message::error("'listen " + host + ":" + port + "' is already defined.");
		}
	}

	listen_type::iterator it = config.listen.find(host);
	if (it != config.listen.end()) {
		(it->second).insert(port);
	} else {
		ports_type ports;
		ports.insert(port);

		config.listen[host] = ports;
	}

	if (this->_extractWord() != ";")
		Message::error("'port' already assigned.");
}

void			ConfigServer::_parseServerNames(configuration_struct &config) {
	std::string word;

	if (config.server_names.size())
		Message::error("'server_names' already assigned.");

	while ((word = this->_extractWord()) != "}" && word != ";")
		config.server_names.push_back(word);

	if (word == "}")
		Message::error("';' is expected.");
}

void			ConfigServer::_parseRoot(configuration_struct &config) {
	if (!config.root.empty())
		Message::error("'root' already assigned.");

	config.root = this->_extractWord();

	if (this->_extractWord() != ";")
		Message::error("'root' already assigned.");
}

void			ConfigServer::_parseIndex(configuration_struct &config) {
	std::string word;

	if (!config.index.empty())
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
	std::string	size_unit;

	if (config.client_max_body_size > -1)
		Message::error("'client_max_body_size' already assigned.");

	size_unit = this->_extractWord();

	if (this->_extractWord() != ";")
		Message::error("'client_max_body_size' already assigned.");

	config.client_max_body_size = fromSizeUnit(size_unit);
}

void			ConfigServer::_parseRedirect(configuration_struct &config) {
	if (!config.redirect.empty())
		Message::error("'redirect' already assigned.");

	config.redirect = this->_extractWord();

	if (this->_extractWord() != ";")
		Message::error("'redirect' already assigned.");
}

void			ConfigServer::_parseErrorPage(configuration_struct &config) {
	int			argument;
	std::string	parameter;
	int			error_code;
	std::string	error_page;

	argument = 0;

	while ((parameter = this->_extractWord()) != "}" && parameter != ";") {
		if (argument >= 2)
			Message::error("Too many arguments for error page.");
		if (argument == 0) {
			error_code = toInteger(parameter);

			if (!isHttpStatus(error_code))
				Message::error("Unknown error '" + parameter + "'.");
		}
		if (argument == 1)
			error_page = parameter;

		argument++;
	}

	if (parameter == "}")
		Message::error("';' is expacted.");

	if (config.error_page.find(error_code) != config.error_page.end())
		Message::error("Error '" + toString(error_code) + "' has already been declared.");

	config.error_page[error_code] = error_page;
}

void		ConfigServer::_parseCGIPath(configuration_struct &config) {
	if (!config.cgi_path.empty())
		Message::error("'cgi_path' already assigned.");

	std::string	tmp = this->_extractWord();

	if (!getAbsolutePath(tmp, config.cgi_path))
		Message::error("'cgi_path " + tmp + ";' not found.");

	if (this->_extractWord() != ";")
		Message::error("'cgi_path' already assigned.");
}

void		ConfigServer::_parseCGIExtentions(configuration_struct &config) {
	std::string	parameter;

	if (!config.cgi_extentions.empty())
		Message::error("cgi_extentions already assigned.");

	while ((parameter = this->_extractWord()) != "}" && parameter != ";")
		config.cgi_extentions.push_back(parameter);

	if (parameter == "}")
		Message::error("';' is expected.");
}

void		ConfigServer::_adjustConfiguration(configuration_struct &config) {
	this->_adjustRoot(config);
	this->_adjustIndex(config);
	this->_adjustErrorPages(config);
	this->_adjustLocations(config);
}

void		ConfigServer::_adjustRoot(configuration_struct &config) {
	std::string	path;

	if (config.root.empty())
		Message::error("Default root not defined.");

	if (!getAbsolutePath(config.root, path))
		Message::error("'" + config.root + "' can't be found.");

	config.root = path;
}

void		ConfigServer::_adjustIndex(configuration_struct &config) {
	if (!config.index.size())
		config.index.push_back(DEFAULT_INDEX);
}

void		ConfigServer::_adjustErrorPages(configuration_struct &config) {
	error_pages_type	error_page;
	std::string		error_path;

	error_pages_type::iterator ite = config.error_page.end();
	for (error_pages_type::iterator it = config.error_page.begin(); it != ite; ++it) {
		if (getAbsolutePath(secureAddress(config.root, it->second), error_path)) {
			error_page[it->first] = error_path;
		}
	}

	config.error_page = error_page;
}

void		ConfigServer::_adjustLocations(configuration_struct &config) {
	std::string	path;

	location_type	location_end = config.locations.end();
	for (location_type location = config.locations.begin(); location != location_end; ++location) {
		if (location->redirect.empty())
			location->redirect = config.redirect;

		path = location->root.empty() ? config.root : location->root;
		if (path.length() > 0 && !getAbsolutePath(path, location->root))
			Message::error("'" + path + "' can't be found.");

		if (location->client_max_body_size == -1)
			location->client_max_body_size = config.client_max_body_size;

		path = location->cgi_path.empty() ? config.cgi_path : location->cgi_path;
		if (path.length() > 0 && !getAbsolutePath(path, location->cgi_path))
			Message::error("'" + path + "' can't be found.");

		std::string		error_path;

		for (error_pages_type::iterator it = config.error_page.begin(); it != config.error_page.end(); it++) {
			if (!location->error_page.count(it->first)) {
				if (getAbsolutePath(secureAddress(config.root, it->second), error_path)) {
					location->error_page[it->first] = error_path;
				}
			}
		}

		if (!location->methods.size())
			location->methods.push_back(METHOD_GET);

		if (location->index.empty())
			location->index = config.index;
	}
}