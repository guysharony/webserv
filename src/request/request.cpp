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
{ this->_header.clear(); }


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

int			Request::getTimeout(void)
{ return (this->_timeout); }

const std::map<std::string, std::string>	&Request::getHeader(void) const
{ return (this->_header); }

int			Request::getStatus(void)
{ return (this->_status); }

int			Request::getEvent(void)
{ return (this->_event); }

int			Request::getClose(void)
{ return (this->_close); }

int			Request::getEnd(void)
{ return (this->_end); }

int			Request::getConnection(void)
{ return (this->_connection); }

int			Request::getLine(void) {
	this->_current.clear();

	std::size_t	end;

	end = this->_temp.find(CRLF);
	if (end != std::string::npos) {
		this->_current = this->_temp.substr(0, end);
		this->_temp = this->_temp.substr(end + 2);
		return (2);
	}

	if (this->getEvent() < EVT_REQUEST_BODY)
		return (0);

	if (!this->_temp.length() || (this->_temp[this->_temp.length() - 1] == '\r')) {
		if (this->_temp.length()) {
			this->_current = this->_temp.substr(0, this->_temp.length() - 1);
			this->_temp = this->_temp.substr(this->_temp.length() - 1);
		}

		return (0);
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

/* Methods */
void			Request::append(std::string packet)
{ this->_temp.append(packet); }

/*
void			Request::parsePathAndVersion(std::string line) {
	size_t	i;

	i = line.find_first_of(' ');
	_path = line.substr(0, i);
	_version = trim2(line.substr(i + 1, line.size() - i - 1));
	checkVersion();
}

void			Request::firstLineParsing(std::string request_buffer)
{
	std::string	line;
	size_t		i;

	i = request_buffer.find(CRLF);
	if (i == std::string::npos) {
		std::cerr << RED << "no newline found!!" << std::endl;
		_ret = STATUS_BAD_REQUEST;
		return;
	}

	line = request_buffer.substr(0, i);
	i = line.find_first_of(' ');
	if (i == std::string::npos)
	{
		std::cerr << RED << "no space found!!" << std::endl;
		_ret = STATUS_BAD_REQUEST;
		return;
	}

	_method = trim2(line.substr(0, i));
	checkMethod();
	line = trim2(line.substr(i, line.size()));
	parsePathAndVersion(line);
}
*/

/*
std::string	Request::getNextLine(std::string str, size_t *i)
{
	std::string	ret;
	size_t		j;

	if (*i == std::string::npos)
		return "";

	j = str.find(CRLF, *i);
	if (j != std::string::npos) {
		ret = str.substr(*i, j - *i);
		*i = j;
	} else {
		ret = str.substr(*i, str.size() - *i - 1);
		*i = str.size() - 1;
	}
	return ret;
}
*/

/*
size_t		Request::headerParsing(std::string request_buffer)
{
	size_t		i;
	std::string	line;
	std::string	key;
	std::string	value;
	size_t		header_length;

	header_length = request_buffer.find(D_CRLF);
	if (header_length == std::string::npos) {
		this->_ret = STATUS_BAD_REQUEST;
		std::cerr << RED << "no header is found!!" << std::endl;
		return -1;
	}

	i = request_buffer.find(CRLF) + 1;
	while (_ret != STATUS_BAD_REQUEST && (line = getNextLine(request_buffer, &i)) != "" && i <= header_length) {
		key = line.substr(0, line.find_first_of(':'));
		value = line.substr(line.find_first_of(':') + 1, line.size() - (line.find_first_of(':') + 1));
		key = trim2(key);
		value = trim2(value);
		this->_header[key] = value;
		i++;
	}
	checkPort();
	checkTimeout();
	return header_length;
}
*/

void			Request::execute(void) {
	Config::configuration_type server;

	this->parseRequest();

	if (this->getEnd()) {
		try {
			server = this->selectServer();
			this->checkBody(server);
		} catch(const Config::ServerNotFoundException & e) {
			Message::debug("Server wasn't found: handling error\n");
			// Handle error here
			// throw e; // delete this once error is handled properly
		}

		if (this->getStatus() < STATUS_BAD_REQUEST) {
			try {
				#ifdef DEBUG
					Config::location_type loc = selectLocation(server);

					if (loc != server->locations.end()) {
						std::cout << YELLOW << "location = " << selectLocation(server)->location << RESET << std::endl;
					}
				#endif
			} catch(const Config::LocationNotFoundException& e) {
					this->setStatus(STATUS_NOT_FOUND);
					std::cerr << RED << "location not found" << RESET << std::endl;
			}
		}
	}
}

void			Request::parseRequest(void) {
	std::size_t	chunk_extention;
	int			res;

	if ((res = this->getLine()) > 0) {
		if (this->getEvent() == EVT_REQUEST_LINE) {
			Message::debug("REQUEST LINE [" + this->_current + "]\n");
			this->_end = 0;
			this->_encoding = NONE;
			this->_content_length = -1;
			this->_status = STATUS_OK;
			this->_chunk_size = -1;

			if (!this->firstLineParsing()) {
				this->setEnd(1);
				return;
			}
		} else if (this->getEvent() == EVT_REQUEST_HEADERS) {
			if (!this->_current.length()) {
				checkPort();
				checkTimeout();
				if (this->_host.empty()) {
					this->setStatus(STATUS_BAD_REQUEST);
					this->setEnd(1);
					return;
				}

				Message::debug("SEPARATOR\n");
				this->_event = EVT_REQUEST_BODY;
				if (this->_encoding == NONE) {
					this->setEnd(1);
					return;
				}
				return;
			}

			Message::debug("REQUEST HEADER [" + this->_current + "]\n");

			if (!this->checkHeaders()) {
				this->setStatus(STATUS_BAD_REQUEST);
				this->setEnd(1);
				return;
			}

		} else if (this->getEvent() == EVT_REQUEST_BODY) {
			if (this->_encoding == CHUNKED) {
				if (!this->_chunked) {
					if (this->_current.length()) {
						chunk_extention = this->_current.find(";");
						if (chunk_extention != std::string::npos)
							this->_current = this->_current.substr(0, chunk_extention);

						if (!isPositiveBase16(this->_current)) {
							this->setStatus(STATUS_BAD_REQUEST);
							this->setEnd(1);
							return;
						}

						this->_chunk_size = hexToInt(this->_current);
						this->_body_size = this->_chunk_size;
						this->_current.clear();
						this->_chunked = true;

						Message::debug("CHUNK SIZE [" + toString(this->_chunk_size) + "]\n");
					}
				} else {
					if (!this->_chunk_size) {
						Message::debug("FINISHED\n");
						this->setEnd(1);
						return;
					}

					this->_body_size -= this->_current.length();

					if (this->_body_size > 0 && res == 2) {
						this->_body_size -= 2;
						this->_current.append("\r\n");
					}

					Message::debug("CHUNK BODY [" + this->_current + "]\n");

					this->_content_length += this->_current.length();
					this->appendTemporary("request", this->_current);

					if (this->_body_size == 0) {
						this->_chunked = false;
					}
				}
			} else if (this->_encoding == LENGTH) {
				if (this->_current.length() > static_cast<size_t>(this->_body_size)) {
					this->setStatus(STATUS_BAD_REQUEST);
					this->setEnd(1);
					return;
				}

				this->_body_size -= this->_current.length();

				if (this->_body_size > 0 && res == 2) {
					this->_body_size -= 2;
					this->_current.append("\r\n");
				}

				Message::debug("LENGTH BODY [" + toString(this->_body_size) + "] - [" + this->_current + "]\n");
				this->appendTemporary("request", this->_current);

				if (this->_body_size == 0) {
					Message::debug("FINISHED\n");
					this->setEnd(1);
					return;
				}
			}
		}
	}
}

std::ostream	&operator<<(std::ostream &os, Request &req) {
	std::map<std::string, std::string>::const_iterator it;

	os << "Method : [" << req.getMethod() << "]" << std::endl;
	os << "path : [" << req.getPath() << "]" << std::endl;
	os << "parameters : [" << req.getParameters() << "]" << std::endl;
	os << "port : [" << req.getPort() << "]" << std::endl;
	os << "host : [" << req.getHost() << "]" << std::endl;
	os << "version : [" << req.getVersion() << "]" << std::endl;
	for (it = req.getHeader().begin(); it != req.getHeader().end(); it++)
		os << "[" << it->first << "] : [" << it->second << "]" << std::endl;
	// os << "body : [" << req.getBody() << "]" << std::endl;
	return os;
}

/*
void			Request::checkMethod(void) {
	if (this->_method.compare("GET") != 0 && this->_method.compare("POST") != 0 && this->_method.compare("DELETE") != 0) {
		this->_ret = STATUS_NOT_ALLOWED;
		std::cerr << RED << "unknown method !!" << std::endl;
	}

	return;
}
*/

void			Request::checkPort(void) {
	size_t		i;
	std::string	tmp;

	i = this->_header["host"].find_first_of(':');
	if (i == std::string::npos)
		this->_port = "80";
	else
	{
		this->_host = this->_header["host"].substr(0, i);
		this->_port = this->_header["host"].substr(i + 1, this->_header["host"].size() - i);
//		if (tmp.size() > 0 && ft_atoi(tmp.c_str()) >= 0 && ft_isalpha(tmp.c_str()) != 1)    si on le veut comme int le port
//			_port = ft_atoi(tmp.c_str());
	}
}

void			Request::checkTimeout(void) {
	std::string tmp;
	tmp = this->_header["Connection-Timeout"];

	if (tmp.size() > 0 && ft_atoi(tmp.c_str()) >= 0 && ft_isalpha(tmp.c_str()) != 1)
		this->_timeout = ft_atoi(tmp.c_str());
}

/*
void			Request::checkVersion(void) {
	size_t		i;
	std::string	str;

	i = this->_version.find_first_of('/');
	str = this->_version.substr(0, i);
	if (str.compare("HTTP") != 0)
	{
		this->_ret = STATUS_BAD_REQUEST;
		std::cerr << RED << "this is not a HTTP version" << std::endl;
		return;
	}

	str = this->_version.substr(i + 1, this->_version.size() - i);
	this->_version = str;
	if (str.compare("1.1") != 0) {
		this->_ret = STATUS_BAD_REQUEST;
		std::cerr << RED << "wrong HTTP version" << std::endl;
		return;
	}
}
*/

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

int			Request::readTemporary(std::string const & filename, std::string & packet)
{ return this->_temporary.read(filename, packet); }

int			Request::displayTemporary(std::string const & filename)
{ return this->_temporary.display(filename); }

int			Request::resetCursorTemporary(std::string const & filename)
{ return this->_temporary.resetCursor(filename); };

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

	if (occurence(this->_current, " ") != 2) {
		this->setStatus(STATUS_INTERNAL_SERVER_ERROR);
		return (0);
	}

	if (!this->checkMethod(this->_current, method)) {
		this->setStatus(STATUS_NOT_ALLOWED);
		return (0);
	}

	if (!this->checkPath(this->_current, path, parameters)) {
		this->setStatus(STATUS_BAD_REQUEST);
		return (0);
	}

	if (!this->checkVersion(this->_current, version)) {
		this->setStatus(STATUS_HTTP_VERSION_NOT_SUPPORTED);
		return (0);
	}

	this->_method = method;
	this->_path = path;
	this->_parameters = parameters;
	this->_version = version;

	this->_event = EVT_REQUEST_HEADERS;

	return (1);
}

int			Request::checkMethod(std::string & source, std::string & dst)
{
	std::string	sep = " ";
	size_t 		pos = source.find(sep);
	std::string	line = source.substr(0, pos);

	source = source.substr(pos + sep.length());

	dst = line;

	if (line.compare("GET") != 0 && line.compare("POST") != 0 && line.compare("DELETE") != 0) {
		return 0;
	}

	return 1;
}

int			Request::checkPath(std::string & source, std::string & path, std::string & parameters)
{
	size_t 		pos = source.find(" ");
	size_t 		par = source.find("?");
	std::string	line = source.substr(0, pos);

	source = source.substr(pos + 1);

	path = line.substr(0, par);
	parameters = ((par != std::string::npos) ? line.substr(par) : "");

	return (1);
}

int			Request::checkVersion(std::string & source, std::string & dst)
{
	if (!source.compare("HTTP/1.1")) {
		dst = source;

		return (1);
	}

	return (0);
}

int			Request::checkHeaders(void)
{
	std::string key;
	std::string value;

	if (this->checkHeader(this->_current, key, value)) {
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

void		Request::displayAllLocations(void) {
	for (Config::configuration_type it = this->_config->configuration.begin(); it != this->_config->configuration.end(); it++) {
		std::cout << it->server_name << std::endl;

		for (Config::location_type it_locations = it->locations.begin(); it_locations != it->locations.end(); it_locations++) {
			std::cout << it_locations->location << std::endl;
		}
	}
}

Config::configuration_type Request::selectServer(void) {
	Config::configuration_type it;
	Config::configuration_type default_server = this->_config->configuration.end();

	for (it = this->_config->configuration.begin(); it != this->_config->configuration.end(); it++) {
		for (Config::listen_type::iterator it2 = it->listen.begin(); it2 != it->listen.end(); it2++) {
			for (Config::ports_type::iterator it3 = it2->second.begin(); it3 != it2->second.end(); it3++) {
				if (!(*it3).compare(this->_port))
				{
					if (default_server == this->_config->configuration.end())
						default_server = it;

					if (!it->server_name.compare(it2->first))
						return it;
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
	Config::location_type	it_location;
	Config::location_type	ret = server->locations.end();
	bool  				firstTime = true;

	std::string tmp = this->_path;
	if (this->_path[this->_path.size() - 1] != '/')
		tmp = this->_path + "/";

	for (it_location = server->locations.begin(); it_location != server->locations.end(); it_location++) {
		if ((it_location->location == "/" || tmp.find(it_location->location + "/") == 0) && (firstTime || it_location->location.size() > ret->location.size())
			&& checkMethodBylocation(it_location->methods)) {
			ret = it_location;
			firstTime = false;
		}
	}

	if (firstTime) //no location found
		throw Config::LocationNotFoundException();

	return (ret);
}

void					Request::checkBody(Config::configuration_type server) {
	ssize_t			max_size = static_cast<ssize_t>(server->client_max_body_size);
	ssize_t			current_size = this->sizeTemporary("request");

	if (max_size >= 0 && current_size > max_size) {
		this->setStatus(STATUS_REQUEST_ENTITY_TOO_LARGE);
		std::cerr << RED << "body too large !! [" << current_size << "] [" << max_size << "]" << std::endl;
	}
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

bool					Request::isCgi(Config::configuration_type server) {
	size_t	i;

	if (!this->_method.compare("POST")) {
		if (server->cgi_path.size() > 0)
			return true;

		this->setStatus(STATUS_INTERNAL_SERVER_ERROR);
		return false;
	}

	i = this->_path.find_last_of(".");
	if (i == std::string ::npos)
		return false;

	std::string ext = this->_path.substr(i, this->_path.size() - 1);
	std::vector<std::string>::iterator it = server->cgi_extentions.begin();
	while (it != server->cgi_extentions.end()) {
		if ((*it).compare(ext) == 0)
			break;
		
		it++;
	}

	return (it != server->cgi_extentions.end() && isFiley(server->root + this->_path) == 1);
}