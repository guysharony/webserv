#include "request.hpp"

Request::Request(Config *config, Descriptors *descriptors)
:
	_method(""),
	_version(""),
	_status(STATUS_OK),
	_path(""),
	_port("80"),
	_host(""),
	_temp(""),
	_current(""),
	_config(config),
	_temporary(descriptors),
	_encoding(NONE),
	_content_length(-1),
	_body_size(-1),
	_chunk_size(-1),
	_close(0),
	_connection(KEEP_ALIVE),
	_chunked(false),
	_event(NONE),
	_timeout(120),
	_end(0)
{
	this->_header.clear();
	this->createTemporary("request");
}

Request::~Request()
{
	this->_temp.clear();
	this->_current.clear();
	this->_header.clear();
}


/* Getters */
std::string	Request::getMethod(void)
{ return (this->_method); }

std::string	Request::getVersion(void)
{ return (this->_version); }

std::string	Request::getPath(void)
{ return (this->_path); }

std::string	Request::getParameters(void)
{ return ((this->_parameters.length() > 0 && this->_parameters[0] == '?') ? this->_parameters.substr(1) : this->_parameters); }

std::string	Request::getURI(void)
{ return (this->_path + this->_parameters); }

std::string	Request::getPort(void)
{ return (this->_port); }

std::string	Request::getHost(void)
{ return (this->_host); }

std::string const	&Request::getClientAddress(void)
{ return (this->_client_address); }

int			Request::getClientPort(void)
{ return (this->_client_port); }

std::string const	&Request::getServerAddress(void)
{ return (this->_server_address); }

int			Request::getServerPort(void)
{ return (this->_server_port); }

int			Request::getTimeout(void)
{ return (this->_timeout); }

const std::map<std::string, std::string>	&Request::getHeader(void) const
{ return (this->_header); }

std::string	Request::getReferer(void)
{ return (this->_header.count("referrer") ? this->_header["referrer"] : "-"); }

std::string	Request::getUserAgent(void)
{ return (this->_header.count("user-agent") ? this->_header["user-agent"] : "-"); }

int			Request::getStatus(void)
{ return (this->_status); }

int			Request::getEvent(void)
{ return (this->_event); }

int			Request::getClose(void)
{ return (this->_close); }

int			Request::getEnd(void)
{ return (this->_end); }

Config		*Request::getConfig(void)
{ return this->_config; }

int			Request::getConnection(void)
{ return (this->_connection); }

int			Request::getLine(void) {
	this->_current.clear();

	std::size_t	end;

	if ((this->getEvent() < EVT_REQUEST_BODY) || (this->getEvent() == EVT_REQUEST_BODY && this->_encoding == CHUNKED && !this->_chunked)) {
		end = this->_temp.find(CRLF);
		if (end != std::string::npos) {
			this->_current = this->_temp.substr(0, end);
			this->_temp = this->_temp.substr(end + 2);
			return (2);
		}

		if (!this->_temp.length() || (this->_temp[this->_temp.length() - 1] == '\r')) {
			if (this->_temp.length()) {
				this->_current = this->_temp.substr(0, this->_temp.length() - 1);
				this->_temp = this->_temp.substr(this->_temp.length() - 1);
			}
		}

		return (0);
	}

	if (this->getEvent() == EVT_REQUEST_BODY && this->_encoding == CHUNKED) {
		this->_current = this->_temp.substr(0, this->_body_size > static_cast<ssize_t>(this->_temp.length()) ? this->_temp.length() : this->_body_size);
		if (this->_body_size > static_cast<ssize_t>(this->_temp.length()))
			this->_temp = this->_temp.substr(this->_current.length());
		else {
			this->_temp = this->_temp.substr(this->_body_size);

			if (this->_body_size > 0 && this->_temp.length() > 0 && (end = this->_temp.find(CRLF)) != 0) {
				this->setStatus(STATUS_BAD_REQUEST);
				this->setEnd(1);
				this->setClose(true);
				return (0);
			}

			this->_temp = this->_temp.substr(2);
		}

		this->_body_size -= this->_current.length();
		return (1);
	}

	this->_current = this->_temp;
	this->_temp.clear();

	return (1);
}


/* Setters */
void			Request::setStatus(int status)
{ this->_status = status; }

void			Request::setEvent(int value)
{ this->_event = value; }

void			Request::setClose(int value)
{ this->_close = value; }

void			Request::setEnd(int value)
{ this->_end = value; };

void			Request::setConnection(int connection)
{ this->_connection = connection; }

void			Request::setClientAddress(std::string address)
{ this->_client_address = address; }

void			Request::setClientPort(int port)
{ this->_client_port = port; }

void			Request::setServerAddress(std::string address)
{ this->_server_address = address; }

void			Request::setServerPort(int port)
{ this->_server_port = port; }


/* Methods */
void			Request::append(std::vector<char> & packet)
{ this->_temp.append(packet); }

void			Request::execute(void) {
	if (!this->getEnd())
		this->parseRequest();

	if (this->getEnd()) {
		try {
			this->selectServer();
		} catch(const Config::ServerNotFoundException & e) {
			this->_status = STATUS_BAD_REQUEST;
		}
	}
}

void			Request::parseRequest(void) {
	std::size_t	chunk_extention;
	int			res;

	if (this->getEvent() < EVT_REQUEST_BODY) {
		while ((res = this->getLine()) > 0) {
			if (this->getEvent() == EVT_REQUEST_LINE) {
				this->_method.clear();
				this->_version.clear();
				this->_status = STATUS_OK;
				this->_path.clear();
				this->_port = "80";
				this->_host.clear();
				this->_encoding = NONE;
				this->_content_length = -1;
				this->_body_size = -1;
				this->_chunk_size = -1;
				this->_close = 0;
				this->_end = 0;
				this->closeTemporary("request");
				this->createTemporary("request");

				if (!this->firstLineParsing()) {
					this->setEnd(1);
					this->setClose(true);
					return;
				}
			} else if (this->getEvent() == EVT_REQUEST_HEADERS) {
				if (!this->_current.length()) {
					checkPort();
					checkTimeout();
					if (this->_host.empty()) {
						this->setStatus(STATUS_BAD_REQUEST);
						this->setEnd(1);
						this->setClose(true);
						return;
					}

					this->_event = EVT_REQUEST_BODY;
					if (this->_encoding == NONE) {
						this->setEnd(1);
						this->setClose(true);
						return;
					}
					return;
				}

				if (!this->checkHeaders()) {
					this->setStatus(STATUS_BAD_REQUEST);
					this->setEnd(1);
					this->setClose(true);
					return;
				}
			}
		}
	} else if (this->getEvent() == EVT_REQUEST_BODY) {
		if ((res = this->getLine()) > 0) {
			if (this->_encoding == CHUNKED) {
				if (!this->_chunked) {
					chunk_extention = this->_current.str().find(";");
					if (chunk_extention != std::string::npos)
						this->_current = this->_current.str().substr(0, chunk_extention);

					if (!isPositiveBase16(this->_current.str())) {
						this->setStatus(STATUS_BAD_REQUEST);
						this->setEnd(1);
						this->setClose(true);
						return;
					}

					this->_chunk_size = hexToInt(this->_current.str());
					this->_body_size = this->_chunk_size;
					this->_current.clear();
					this->_chunked = true;
				} else {
					if (!this->_chunk_size) {
						this->setEnd(1);
						return;
					}

					this->appendTemporary("request", this->_current);
					if (this->_body_size <= 0) {
						this->_chunked = false;
						return;
					}
				}
			} else if (this->_encoding == LENGTH) {
				if (this->_current.length() > static_cast<size_t>(this->_body_size)) {
					this->setStatus(STATUS_BAD_REQUEST);
					this->setEnd(1);
					this->setClose(true);
					return;
				}

				this->_body_size -= this->_current.length();

				this->appendTemporary("request", this->_current);

				if (this->_body_size <= 0) {
					this->setEnd(1);
					return;
				}
			}
		}
	}
}

void			Request::checkPort(void) {
	size_t		i;
	std::string	tmp;

	i = this->_header["host"].find_first_of(':');
	if (i != std::string::npos)
		this->_host = this->_header["host"].substr(0, i);
	else
		this->_host = this->_header["host"];

	this->_port = toString(this->_server_port);
}

void			Request::checkTimeout(void) {
	std::string tmp;

	if (this->_header.count("connection-timeout")) {
		tmp = this->_header["connection-timeout"];

		if (tmp.size() > 0 && toInteger(tmp) >= 0 && isAlpha(tmp) != 1)
			this->_timeout = toInteger(tmp);
	}
}


/* Temporary */
int			Request::createTemporary(std::string const & filename)
{ return this->_temporary.create(filename); }

int			Request::eventTemporary(std::string const & filename, short event)
{ return this->_temporary.setEvents(filename, event); }

int			Request::fdTemporary(std::string const & filename)
{ return this->_temporary.getFD(filename); }

int			Request::appendTemporary(std::string const & filename, std::string packet)
{
	if (!(this->_temporary.getEvents(filename) & POLLOUT)) {
		this->_temporary.setEvents(filename, POLLOUT);
		return 0;
	}

	this->_temporary.append(filename, packet);
	return 1;
}

int			Request::appendTemporary(std::string const & filename, STRBinary const & packet)
{
	if (!(this->_temporary.getEvents(filename) & POLLOUT)) {
		this->_temporary.setEvents(filename, POLLOUT);
		return 0;
	}

	this->_temporary.append(filename, packet);
	return 1;
}

int			Request::readTemporary(std::string const & filename, STRBinary & packet)
{ return this->_temporary.read(filename, packet); }

int			Request::resetCursorTemporary(std::string const & filename)
{ return this->_temporary.resetCursor(filename); };

int			Request::clearTemporary(std::string const & filename)
{ return this->_temporary.clear(filename); };

ssize_t		Request::sizeTemporary(std::string const & filename)
{ return this->_temporary.getSize(filename); };

int			Request::closeTemporary(std::string const & filename)
{ return this->_temporary.close(filename); }


int			Request::firstLineParsing(void)
{
	std::string	method;
	std::string	path;
	std::string	parameters;
	std::string	version;

	std::string	tmp;

	tmp = this->_current.str();

	if (tmp.length() >= 2047) {
		this->setStatus(STATUS_REQUEST_URI_TOO_LARGE);
		return (0);
	}

	if (occurence(tmp, " ") != 2) {
		this->setStatus(STATUS_BAD_REQUEST);
		return (0);
	}

	this->checkMethod(tmp);
	this->checkPath(tmp);
	this->checkVersion(tmp);

	if (this->getMethod().compare("GET")
	&& this->getMethod().compare("POST")
	&& this->getMethod().compare("DELETE"))
		this->setStatus(STATUS_NOT_ALLOWED);

	if (this->getStatus() == STATUS_OK && this->getVersion().compare("HTTP/1.1"))
		this->setStatus(STATUS_HTTP_VERSION_NOT_SUPPORTED);

	this->_event = EVT_REQUEST_HEADERS;

	return (1);
}

int			Request::checkMethod(std::string & source)
{
	std::string	sep = " ";
	size_t 		pos = source.find(sep);
	std::string	line = source.substr(0, pos);

	source = source.substr(pos + sep.length());

	this->_method = line;

	return (1);
}

int			Request::checkPath(std::string & source)
{
	size_t 		pos;
	size_t 		par;
	std::string	line;

	pos = source.find(" ");
	par = source.find("?");
	line = source.substr(0, pos);

	source = source.substr(pos + 1);

	this->_path = line.substr(0, par);
	this->_parameters = ((par != std::string::npos) ? line.substr(par) : "");

	return (1);
}

int			Request::checkVersion(std::string & source)
{
	this->_version = source;

	return (1);
}

int			Request::checkHeaders(void)
{
	std::string key;
	std::string value;

	if (this->_current.length() >= 7999)
		return 0;

	if (this->checkHeader(this->_current.str(), key, value)) {
		if (!key.compare("connection")) {
			this->_connection = KEEP_ALIVE;

			if (!value.compare("close")) this->_connection = CLOSE;
			if (!value.compare("keep-alive")) this->_connection = KEEP_ALIVE;
		}
		else if (!key.compare("content-length")) {
			if (this->_content_length >= 0 || !isPositiveBase10(value))
				return 0;

			this->_encoding = LENGTH;
			this->_content_length = toInteger(value);
			this->_body_size = this->_content_length;
		}
		else if (!key.compare("transfer-encoding")) {
			if (!value.compare("chunked")) {
				this->_encoding = CHUNKED;
				this->_content_length = 0;
				this->_body_size = 0;
				this->_chunked = 0;
			}
		}
		else {
			this->_header[key] = value;
		}
	}

	return 1;
}

int			Request::checkHeader(std::string source, std::string & key, std::string & value)
{
	size_t	pos = source.find(":");

	std::string tmp_key;
	std::string tmp_value;

	if (pos != std::string::npos)
	{
		tmp_key = source.substr(0, pos);
		tmp_value = source.substr(pos + 1);

		if (!isTchar(tmp_key))
			return (0);

		trim(tmp_value);

		key = toLowercase(tmp_key);
		value = tmp_value;

		return (1);
	}

	return (0);
}

Config::configuration_type Request::selectServer(void) {
	Config::configuration_type ite = this->_config->configuration.end();
	Config::configuration_type default_server = ite;

	for (Config::configuration_type it = this->_config->configuration.begin(); it != ite; ++it)
	{
		Config::listen_type::iterator it2e = it->listen.end();
		for (Config::listen_type::iterator it2 = it->listen.begin(); it2 != it2e; ++it2)
		{
			Config::ports_type::iterator it3e = it2->second.end();
			for (Config::ports_type::iterator it3 = it2->second.begin(); it3 != it3e; ++it3)
			{
				if (!(*it3).compare(this->_port))
				{
					if (default_server == ite)
						default_server = it;

					std::vector<std::string>::iterator it4e = it->server_names.end();
					for (std::vector<std::string>::iterator it4 = it->server_names.begin(); it4 != it4e; ++it4)
					{
						if (!(*it4).compare(this->_host))
							return it;
					}
				}
			}
		}
	}

	if (default_server == this->_config->configuration.end())
		throw Config::ServerNotFoundException();

	return default_server;
}

bool					Request::checkMethodBylocation(std::vector<int> methods_type)
{ return std::find(methods_type.begin(), methods_type.end(), convertMethodToValue(this->_method)) != methods_type.end(); }

Config::location_type	Request::selectLocation(Config::configuration_type server) {
	Config::location_type	ret = server->locations.end();
	bool  				firstTime = true;

	std::string tmp = this->_path;
	if (this->_path[this->_path.size() - 1] != '/')
		tmp = this->_path + "/";

	Config::location_type ite = server->locations.end();
	for (Config::location_type it = server->locations.begin(); it != ite; ++it) {
		if ((it->location == "/" || tmp.find(it->location + "/") == 0)
		&& (firstTime || it->location.size() > ret->location.size())) {
			ret = it;
			firstTime = false;
		}
	}

	if (firstTime)
		throw Config::LocationNotFoundException();

	if (!checkMethodBylocation(ret->methods))
		throw Config::MethodNotAllowed();

	return (ret);
}

int					Request::convertMethodToValue(std::string method) {
	if (method.compare("GET") == 0)
		return METHOD_GET;
	if (method.compare("HEAD") == 0)
		return METHOD_HEAD;
	if (method.compare("POST") == 0)
		return METHOD_POST;
	if (method.compare("PUT") == 0)
		return METHOD_PUT;
	if (method.compare("DELETE") == 0)
		return METHOD_DELETE;
	return 0;
}

bool					Request::isCgi(Config::configuration_type server, std::string path) {
	size_t	i;

	if (!server->cgi_path.size())
		return false;

	i = path.find_last_of(".");
	if (i == std::string ::npos)
		return false;

	std::string ext = path.substr(i, path.size() - 1);
	std::vector<std::string>::iterator it = server->cgi_extentions.begin();
	while (it != server->cgi_extentions.end()) {
		if ((*it).compare(ext) == 0)
			break;
		
		it++;
	}
	
	if (it != server->cgi_extentions.end()) {
		if (server->cgi_path.size() == 0)
			this->setStatus(STATUS_INTERNAL_SERVER_ERROR);
		return true;
	}
	return false;

}
