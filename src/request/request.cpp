#include "request.hpp"

request::request(void){
    this->_method = "";
    this->_path = "";
    this->_version = "";
    this->_body = "";
    this->_port = 80;
    this->_header.clear(); 
    this->_ret = 0;
}

void    request::request_clear(){
    this->_method = "";
    this->_path = "";
    this->_version = "";
    this->_body = "";
    this->_port = 80;
    this->_header.clear(); 
}

request::~request(void){}

std::string request::getMethod(){
    return(this->_method);
}

std::string request::getVersion(){
    return(this->_version);
}

std::string request::getPath(){
    return(this->_path);
}

int request::getPort(){
    return(this->_port);
}

const std::map<std::string, std::string>& request::getHeader() const{
    return (this->_header);
}

std::string request::getBody(){
    return(this->_body);
}

void    request::parsePathAndVersion(std::string line){
    size_t i;

    i = line.find_first_of(' ');
    _path = line.substr(0, i);
    _version = trim(line.substr(i + 1, line.size() - i - 2));
    checkVersion();
}

void request::firstLineParsing(std::string request_buffer){
    std::string line;
    size_t i;

    i = request_buffer.find_first_of('\n');
    if ( i == std::string::npos ){
        std::cerr<< RED<<"no newline found!!"<< std::endl;
        _ret = -1;
        request_clear();
        return ;
    }
    line = request_buffer.substr(0, i);
    i = line.find_first_of(' ');
    if ( i == std::string::npos ){
        std::cerr <<RED<< "no space found!!" << std::endl;
        _ret = -1;
        request_clear();
        return ;
    }
    _method = trim(line.substr(0, i));
    checkMethod();
    line = trim(line.substr(i, line.size()));
    parsePathAndVersion(line);
}

std::string			request::getNextLine(std::string str, size_t *i)
{
	std::string		ret;
	size_t			j;

	if (*i == std::string::npos)
		return "";
	j = str.find_first_of('\n', *i);
	ret = str.substr(*i, j - *i);
    if (j == std::string::npos)
        *i = j;
    else
        *i = j+ 1;
	return ret;
}

size_t request::headerParsing(std::string request_buffer){

    size_t i;
    std::string line;
    std::string key;
    std::string value;
    size_t      header_length;
    
    header_length = request_buffer.find("\r\n\r\n");
    if (header_length == std::string::npos){
        _ret = -1;
        std::cerr<<RED<<"no header is found!!"<<std::endl;
        request_clear();
        return -1;
    }
    i = request_buffer.find_first_of('\n') + 1;
    while (_ret != -1  && (line = getNextLine(request_buffer, &i)) != "" && i < header_length){
        key = line.substr(0, line.find_first_of(':'));
        value = line.substr(line.find_first_of(':') + 1, line.size() - (line.find_first_of(':') + 1) - 1);
        key = trim(key);
        value = trim(value);
		this->_header[key] = value;
    }
    checkPort();
    return header_length;

}

void request::parseRequest(std::string request_buffer){
    size_t i;
    if (_ret != -1)
        firstLineParsing(request_buffer);
    if (_ret != -1)
        i = headerParsing(request_buffer);
    if (_ret != -1)
        this->_body = trim(request_buffer.substr(i, request_buffer.size() - i));
}

std::ostream&		operator<<(std::ostream& os,request& req)
{
	std::map<std::string, std::string>::const_iterator	it;

	os << "Method : [" << req.getMethod()<<"]" << std::endl ;
	os << "path : [" << req.getPath() <<"]"<< std::endl;
	os << "port : [" << req.getPort() <<"]"<< std::endl;
	os << "version : [" << req.getVersion()<<"]" << std::endl;
	for (it = req.getHeader().begin(); it != req.getHeader().end(); it++)
		os <<"["<< it->first << "] : [" << it->second<<"]" << std::endl;
    os << "body : ["<<req.getBody()<<"]"<<std::endl;
	return os;
}

void request::checkMethod(){
    if (_method.compare("GET") != 0 && _method.compare("POST") != 0 && _method.compare("DELETE") != 0 ){
        _ret = -1;
        request_clear();
        std::cerr<<RED<< "unknown method !!"<<std::endl;
    }
    return ;
}


void request::checkPort(){
    size_t i;
    std::string tmp;

    i = this->_header["Host"].find_first_of(':');
    if (i == std::string::npos)
		this->_port = 80;
	else{
		tmp = _header["Host"].substr(i + 1, _header["Host"].size() - i);
        if (tmp.size() > 0 && ft_atoi(tmp.c_str()) >= 0 && ft_isalpha(tmp.c_str()) != 1)
            _port = ft_atoi(tmp.c_str());
        else{
            _ret = -1;
            request_clear();
            std::cerr<<RED<<"unknown port !"<<std::endl;
            return ;
        }      
	}
}

void request::checkVersion(){
    size_t i;
    std::string str;

    i = _version.find_first_of('/');
    str = _version.substr(0, i);
    if (str.compare("HTTP") != 0){
        _ret = -1;
        request_clear();
        std::cerr<<RED<<"this is not a HTTP version"<<std::endl;
        return ;
    }
    str = _version.substr(i + 1, _version.size() - i - 1);
    _version = str;
    if(str.compare("1.1") != 0){
        _ret = -1;
        request_clear();
        std::cerr<<RED<<"wrong HTTP version"<<std::endl;
        return ;
    }
}