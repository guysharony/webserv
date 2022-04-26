#include "configLocation.hpp"

ConfigLocation::ConfigLocation(void)
:
	compressed_configuration_file("")
{ }

ConfigLocation::ConfigLocation(ConfigLocation const & src)
{ *this = src; }

ConfigLocation::~ConfigLocation()
{ }

void			ConfigLocation::parseLocation(configuration_struct &config) {
	location_struct	location;
	std::string		word;

	location.auto_index = 0;
	location.client_max_body_size = -1;

	word = this->_extractWord();

	if (word[0] != '/')
		Message::error("Path expected in location block.");

	location.location = word;

	if (this->_extractWord() != "{")
		Message::error("Path already assigned in location block.");

	while ((word = this->_extractWord()) != "}") {
		if (word == "methods") {
			this->_parseMethods(location);
		} else if (word == "root") {
			this->_parseRoot(location);
		} else if (word == "index") {
			this->_parseIndex(location);
		} else if (word == "auto_index") {
			this->_parseAutoIndex(location);
		} else if (word == "client_max_body_size") {
			this->_parseClientMaxBodySize(location);
		} else if (word == "redirect") {
			this->_parseRedirect(location);
		} else if (word == "error_page") {
			this->_parseErrorPage(location);
		} else if (word == "cgi_path") {
			this->_parseCGIPath(location);
		} else if (word == "cgi_extentions") {
			this->_parseCGIExtentions(location);
		} else {
			Message::error("Unknown directive: " + word);
		}
	}

	config.locations.push_back(location);
}

std::string 	ConfigLocation::_extractWord(void) {
	std::string	res;

	while (this->compressed_configuration_file_iterator < this->compressed_configuration_file.end() && *this->compressed_configuration_file_iterator == ' ')
		this->compressed_configuration_file_iterator++;

	while (this->compressed_configuration_file_iterator < this->compressed_configuration_file.end() && *this->compressed_configuration_file_iterator != ' ') {
		res.push_back(*this->compressed_configuration_file_iterator);
		this->compressed_configuration_file_iterator++;
	}

	return (res);
}

void			ConfigLocation::_parseMethods(location_struct &location) {
	std::string 	method_string;
	int			method_code;

	while ((method_string = this->_extractWord()) != ";")
	{
		if (!isHttpMethod(method_string, method_code))
			Message::error("Unknown method '" + method_string + "'.");

		location.methods.push_back(method_code);
	}
}

void			ConfigLocation::_parseRoot(location_struct &location) {
	if (!location.root.empty())
		Message::error("'root' already assigned.");

	location.root = this->_extractWord();

	if (this->_extractWord() != ";")
		Message::error("'root' already assigned.");
}

void			ConfigLocation::_parseIndex(location_struct &location) {
	std::string path;

	if (!location.index.empty())
		Message::error("Index already assigned.");

	while ((path = this->_extractWord()) != "}" && path != ";")
		location.index.push_back(path);

	if (path == "}")
		Message::error("';' is expacted.");
}

void			ConfigLocation::_parseAutoIndex(location_struct &location) {
	std::string	auto_index;

	auto_index = this->_extractWord();

	if (auto_index == "on")
		location.auto_index = 1;
	else if (auto_index == "off")
		location.auto_index = 0;
	else
		Message::error("'auto_index' cannot be set to '" + auto_index + "'.");

	if (this->_extractWord() != ";")
		Message::error("';' is expected.");
}

void			ConfigLocation::_parseClientMaxBodySize(location_struct &location) {
	std::string	size_unit;

	if (location.client_max_body_size > -1)
		Message::error("'client_max_body_size' already assigned.");

	size_unit = this->_extractWord();

	if (this->_extractWord() != ";")
		Message::error("'client_max_body_size' already assigned.");

	location.client_max_body_size = fromSizeUnit(size_unit);
}

void			ConfigLocation::_parseRedirect(location_struct &location) {
	if (!location.redirect.empty())
		Message::error("'redirect' already assigned.");

	location.redirect = this->_extractWord();

	if (this->_extractWord() != ";")
		Message::error("'redirect' already assigned.");
}

void			ConfigLocation::_parseErrorPage(location_struct &location) {
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

	if (location.error_page.find(error_code) != location.error_page.end())
		Message::error("Error '" + toString(error_code) + "' has already been declared.");

	location.error_page[error_code] = error_page;
}

void		ConfigLocation::_parseCGIPath(location_struct &location) {
	if (!location.cgi_path.empty())
		Message::error("'cgi_path' already assigned.");

	std::string	tmp = this->_extractWord();

	if (!getAbsolutePath(tmp, location.cgi_path))
		Message::error("'cgi_path " + tmp + ";' not found.");

	if (this->_extractWord() != ";")
		Message::error("'cgi_path' already assigned.");
}

void		ConfigLocation::_parseCGIExtentions(location_struct &location) {
	std::string	parameter;

	if (!location.cgi_extentions.empty())
		Message::error("cgi_extentions already assigned.");

	while ((parameter = this->_extractWord()) != "}" && parameter != ";")
		location.cgi_extentions.push_back(parameter);

	if (parameter == "}")
		Message::error("';' is expected.");
}