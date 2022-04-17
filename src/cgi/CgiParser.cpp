#include "CgiParser.hpp"

CgiParser::CgiParser(){
    this->_headers.clear();
    this->_body = "";
    this->_status = 200;

}

CgiParser::~CgiParser()
{
}

CgiParser &CgiParser::operator=(CgiParser const & rhs)
{
	if (this != &rhs){
		this->_headers = rhs._headers;
		this->_body = rhs._body;
		this->_status = rhs._status;
	}
	return (*this);
}

CgiParser::CgiParser(CgiParser const & src) {
	*this = src;
}

int CgiParser::getStatus() {
	return( this->_status);
}

const std::map<std::string, std::string> &CgiParser::getHeaders() const {
	return(this->_headers);
}

std::string	CgiParser::getBody() {
	return(this->_body);
}

std::string	CgiParser::getNextLine(std::string str, size_t *i) {
	std::string ret;
	size_t j;

	if (*i == std::string::npos)
		return "";

	j = str.find(CRLF, *i);
	if (j != std::string::npos) {
		ret = str.substr(*i, j - *i);
		*i = j;
	}
	else
	{
		ret = str.substr(*i, str.size() - *i - 1);
		*i = str.size() - 1;
	}
	return ret;
}

size_t CgiParser::parseHeaders(std::string buffer) {
	size_t i;
	std::string line;
	std::string key;
	std::string value;
	size_t header_length;

	header_length = buffer.find(D_CRLF);
	if (header_length == std::string::npos){
		_status = STATUS_BAD_REQUEST;
		std::cerr << RED << "no header cgi is found!!" << std::endl;
		return -1;
	}
	i = 0;
	while (_status != STATUS_BAD_REQUEST && (line = getNextLine(buffer, &i)) != "" && i <= header_length){
		key = line.substr(0, line.find_first_of(':'));
		value = line.substr(line.find_first_of(':') + 1, line.size() - (line.find_first_of(':') + 1));
		key = trim2(key);
		value = trim2(value);
		this->_headers[key] = value;
		i++;
	}
	return header_length;
}

void CgiParser::parseStat() {
	for (std::map<std::string, std::string>::iterator it = _headers.begin(); it != _headers.end(); it++){
		if (it->first.compare("Status") == 0){
			this->_status = atoi(it->second.c_str());
		}
	}
}

void CgiParser::parseCgiBuffer(std::string buffer) {
	size_t i;

	i = parseHeaders(buffer);
	parseStat();
	this->_body = trim2(buffer.substr(i, buffer.size() - i));
	std::cout << "body =" << _body << std::endl;
}
