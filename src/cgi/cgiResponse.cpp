#include "cgiResponse.hpp"

CgiResponse::CgiResponse(){
    this->_headers.clear();
    this->_body = "";
    this->_status = 200;

}

CgiResponse::~CgiResponse()
{
}

CgiResponse &CgiResponse::operator=(CgiResponse const & rhs)
{
	if (this != &rhs){
		this->_headers = rhs._headers;
        this->_body = rhs._body;
        this->_status = rhs._status;
	}
	return (*this);
}
CgiResponse::CgiResponse(CgiResponse const & src){
    *this = src;
}

std::string CgiResponse::getNextLine(std::string str, size_t *i)
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

size_t CgiResponse::parseHeaders(std::string buffer){
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
	i = buffer.find(CRLF) + 1;
	while (_status != STATUS_BAD_REQUEST && (line = getNextLine(buffer, &i)) != "" && i < header_length){
		key = line.substr(0, line.find_first_of(':'));
		value = line.substr(line.find_first_of(':') + 1, line.size() - (line.find_first_of(':') + 1));
		key = trim2(key);
		value = trim2(value);
		this->_headers[key] = value;
	}
    return header_length;
}

void CgiResponse::parseStat(){
    for (std::map<std::string, std::string>::iterator it = _headers.begin(); it != _headers.end(); it++){
       if (it->first.compare("Status") == 0)
            this->_status = atoi(it->second.c_str());
    }
}

void CgiResponse::parseCgiBuffer(std::string buffer){
    size_t i;
    
    i = parseHeaders(buffer);
    parseStat();
    this->_body = trim2(buffer.substr(i, buffer.size() - i));
}
