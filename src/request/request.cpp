#include "request.hpp"

request::request(void)
{
	this->_method = "";
	this->_host = "";
	this->_path = "";
	this->_version = "";
	this->_body = "";
	this->_port = "80";
	this->_header.clear();
	this->_ret = STATUS_OK;
}

request::request(request const &src){
	*this = src;
}

request::request(Config& conf){
	this->_method = "";
	this->_host = "";
	this->_path = "";
	this->_version = "";
	this->_body = "";
	this->_port = "80";
	this->_header.clear();
	this->_ret = STATUS_OK;
	this->_config = conf;
}


request &request::operator=(request const &rhs){
	if (this != &rhs)
	{
		this->_body = rhs._body;
		this->_port = rhs._port;
		this->_host = rhs._host;
		this->_header = rhs._header;
		this->_method = rhs._method;
		this->_path = rhs._path;
		this->_ret = rhs._ret;
		this->_version = rhs._version;
		this->_config = rhs._config;
	}
	return (*this);
}

void request::request_clear()
{
	this->_method = "";
	this->_path = "";
	this->_host = "";
	this->_version = "";
	this->_body = "";
	this->_port = "80";
	this->_ret = STATUS_OK;
	this->_header.clear();
}

request::~request(void) {
	request_clear();
}

std::string request::getMethod(void)
{
	return (this->_method);
}

std::string request::getVersion(void)
{
	return (this->_version);
}

std::string request::getPath(void)
{
	return (this->_path);
}

std::string request::getPort(void)
{
	return (this->_port);
}

std::string request::getHost(void)
{
	return (this->_host);
}

const std::map<std::string, std::string> &request::getHeader() const
{
	return (this->_header);
}

std::string request::getBody(void)
{
	return (this->_body);
}

int request::getRet(void)
{
	return (this->_ret);
}

void request::parsePathAndVersion(std::string line)
{
	size_t i;

	i = line.find_first_of(' ');
	_path = line.substr(0, i);
	_version = trim2(line.substr(i + 1, line.size() - i - 1));
	checkVersion();
}

void request::firstLineParsing(std::string request_buffer)
{
	std::string line;
	size_t i;

	i = request_buffer.find(CRLF);
	if (i == std::string::npos)
	{
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

std::string request::getNextLine(std::string str, size_t *i)
{
	std::string ret;
	size_t j;

	if (*i == std::string::npos)
		return "";
	j = str.find(CRLF, *i);
	ret = str.substr(*i, j - *i);
	if (j == std::string::npos)
		*i = j;
	else
		*i = j + 1;
	return ret;
}

size_t request::headerParsing(std::string request_buffer)
{

	size_t i;
	std::string line;
	std::string key;
	std::string value;
	size_t header_length;

	header_length = request_buffer.find(D_CRLF);
	if (header_length == std::string::npos)
	{
		_ret = STATUS_BAD_REQUEST;
		std::cerr << RED << "no header is found!!" << std::endl;
		return -1;
	}
	i = request_buffer.find(CRLF) + 1;
	while (_ret != STATUS_BAD_REQUEST && (line = getNextLine(request_buffer, &i)) != "" && i < header_length)
	{
		key = line.substr(0, line.find_first_of(':'));
		value = line.substr(line.find_first_of(':') + 1, line.size() - (line.find_first_of(':') + 1));
		key = trim2(key);
		value = trim2(value);
		this->_header[key] = value;
	}
	checkPort();
	return header_length;
}

void request::parseRequest(std::string request_buffer)
{
	size_t i;
	if (_ret < STATUS_BAD_REQUEST)
		firstLineParsing(request_buffer);
	if (_ret < STATUS_BAD_REQUEST)
		i = headerParsing(request_buffer);
	if (_ret < STATUS_BAD_REQUEST){
		this->_body = trim2(request_buffer.substr(i, request_buffer.size() - i));
		Config::configuration_struct server = selectServer();
		checkBody(server);
		if (_ret < STATUS_BAD_REQUEST)
			std::cout<<YELLOW<< "location = "<<selectLocation(server).location<<RESET<<std::endl;
	}
}

std::ostream &operator<<(std::ostream &os, request &req)
{
	std::map<std::string, std::string>::const_iterator it;

	os << "Method : [" << req.getMethod() << "]" << std::endl;
	os << "path : [" << req.getPath() << "]" << std::endl;
	os << "port : [" << req.getPort() << "]" << std::endl;
	os << "host : [" << req.getHost() << "]" << std::endl;
	os << "version : [" << req.getVersion() << "]" << std::endl;
	for (it = req.getHeader().begin(); it != req.getHeader().end(); it++)
		os << "[" << it->first << "] : [" << it->second << "]" << std::endl;
	os << "body : [" << req.getBody() << "]" << std::endl;
	return os;
}

void request::checkMethod()
{
	if (_method.compare("GET") != 0 && _method.compare("POST") != 0 && _method.compare("DELETE") != 0)
	{
		_ret = STATUS_NOT_ALLOWED;
		std::cerr << RED << "unknown method !!" << std::endl;
	}
	return;
}

void request::checkPort()
{
	size_t i;
	std::string tmp;

	i = this->_header["Host"].find_first_of(':');
	if (i == std::string::npos)
		this->_port = "80";
	else
	{
		_host = _header["Host"].substr(0, i);
		_port = _header["Host"].substr(i + 1, _header["Host"].size() - i);
//		if (tmp.size() > 0 && ft_atoi(tmp.c_str()) >= 0 && ft_isalpha(tmp.c_str()) != 1)    si on le veut comme int le port
//			_port = ft_atoi(tmp.c_str());
	}
}

void request::checkVersion()
{
	size_t i;
	std::string str;

	i = _version.find_first_of('/');
	str = _version.substr(0, i);
	if (str.compare("HTTP") != 0)
	{
		_ret = STATUS_BAD_REQUEST;
		std::cerr << RED << "this is not a HTTP version" << std::endl;
		return;
	}
	str = _version.substr(i + 1, _version.size() - i);
	_version = str;
	if (str.compare("1.1") != 0)
	{
		_ret = STATUS_BAD_REQUEST;
		std::cerr << RED << "wrong HTTP version" << std::endl;
		return;
	}
}

void request::displayAllLocations(void){
	for (Config::configuration_type it = this->_config.configuration.begin(); it != this->_config.configuration.end(); it++) {
			std::cout<<it->server_name<<std::endl;
		for(Config::location_type it_locations = it->locations.begin(); it_locations != it->locations.end(); it_locations++){
			std::cout<<it_locations->location<<std::endl;
		}
	}

}

Config::configuration_struct &request::selectServer(){
	Config::configuration_type it;
	Config::configuration_type default_server = this->_config.configuration.end();
	for (it = this->_config.configuration.begin(); it != this->_config.configuration.end(); it++) {
		if (it->port.compare(this->_port) == 0)
		{
			if (default_server == this->_config.configuration.end())
			{
				default_server = it;
			}
			if (it->server_name.compare(this->_host) == 0)
			{
				return (*it);
			}
		}
	}
	return (*default_server);
}

Config::location_struct &request::selectLocation(Config::configuration_struct &server){
	Config::location_type it_location;
	Config::location_type ret;
	bool  				firstTime = true;

	for(it_location = server.locations.begin(); it_location != server.locations.end(); it_location++){
		if (this->_path.find(it_location->location) == 0 && (firstTime || it_location->location.size() > ret->location.size())){
			ret = it_location;			
			firstTime = false;
		}
	}
	return (*ret);
}

void  request::checkBody(Config::configuration_struct &server){
	if (_body.size() > (size_t)server.client_max_body_size){
		_ret = STATUS_REQUEST_ENTITY_TOO_LARGE;
		std::cerr << RED << "body too large !!" << std::endl;
		return;
	}

}



