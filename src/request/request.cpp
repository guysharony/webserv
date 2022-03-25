#include "request.hpp"

request::request(void)
{
	this->_method = "";
	this->_path = "";
	this->_version = "";
	this->_body = "";
	this->_port = 80;
	this->_header.clear();
	this->_ret = 0;
}

request::request(request const &src){
	*this = src;
}

request::request(Config& conf){
	request();
	this->_config = conf;
}


request &request::operator=(request const &rhs){
	if (this != &rhs)
	{
		this->_body = rhs._body;
		this->_port = rhs._port;
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
	this->_version = "";
	this->_body = "";
	this->_port = 80;
	this->_header.clear();
}

request::~request(void) {}

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

int request::getPort(void)
{
	return (this->_port);
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
	_version = trim(line.substr(i + 1, line.size() - i - 2));
	checkVersion();
}

void request::firstLineParsing(std::string request_buffer)
{
	std::string line;
	size_t i;

	i = request_buffer.find_first_of('\n');
	if (i == std::string::npos)
	{
		std::cerr << RED << "no newline found!!" << std::endl;
		_ret = STATUS_BAD_REQUEST;
		request_clear();
		return;
	}
	line = request_buffer.substr(0, i);
	i = line.find_first_of(' ');
	if (i == std::string::npos)
	{
		std::cerr << RED << "no space found!!" << std::endl;
		_ret = STATUS_BAD_REQUEST;
		request_clear();
		return;
	}
	_method = trim(line.substr(0, i));
	checkMethod();
	line = trim(line.substr(i, line.size()));
	parsePathAndVersion(line);
}

std::string request::getNextLine(std::string str, size_t *i)
{
	std::string ret;
	size_t j;

	if (*i == std::string::npos)
		return "";
	j = str.find_first_of('\n', *i);
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

	header_length = request_buffer.find("\r\n\r\n");
	if (header_length == std::string::npos)
	{
		_ret = STATUS_BAD_REQUEST;
		std::cerr << RED << "no header is found!!" << std::endl;
		request_clear();
		return -1;
	}
	i = request_buffer.find_first_of('\n') + 1;
	while (_ret != STATUS_BAD_REQUEST && (line = getNextLine(request_buffer, &i)) != "" && i < header_length)
	{
		key = line.substr(0, line.find_first_of(':'));
		value = line.substr(line.find_first_of(':') + 1, line.size() - (line.find_first_of(':') + 1) - 1);
		key = trim(key);
		value = trim(value);
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
	if (_ret < STATUS_BAD_REQUEST)
		this->_body = trim(request_buffer.substr(i, request_buffer.size() - i));
	displayAllLocations();
}

std::ostream &operator<<(std::ostream &os, request &req)
{
	std::map<std::string, std::string>::const_iterator it;

	os << "Method : [" << req.getMethod() << "]" << std::endl;
	os << "path : [" << req.getPath() << "]" << std::endl;
	os << "port : [" << req.getPort() << "]" << std::endl;
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
		request_clear();
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
		this->_port = 80;
	else
	{
		tmp = _header["Host"].substr(i + 1, _header["Host"].size() - i);
		if (tmp.size() > 0 && ft_atoi(tmp.c_str()) >= 0 && ft_isalpha(tmp.c_str()) != 1)
			_port = ft_atoi(tmp.c_str());
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
		request_clear();
		std::cerr << RED << "this is not a HTTP version" << std::endl;
		return;
	}
	str = _version.substr(i + 1, _version.size() - i - 1);
	_version = str;
	if (str.compare("1.1") != 0)
	{
		_ret = STATUS_BAD_REQUEST;
		request_clear();
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
