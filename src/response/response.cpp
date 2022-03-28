#include "response.hpp"



std::string		response::getResponse(void)
{
	return (this->_response);
}


response & response::operator=(const response & src)
{
	_response = src._response;
	_path = src._path;
	_codeDeRetour = src._codeDeRetour;
    _autoIndex = src._autoIndex;
	return (*this);
}


response::response(void){}

response::response(const response & src)
{
	*this = src;
}

response::~response(void){}

response::response(request & request, Config& conf){
    this->_codeDeRetour = request.getRet();
    this->_response = "";
    Config::configuration_struct    Server = request.selectServer();
    this->_path = request.getPath();
    this->_autoIndex = request.selectLocation(Server)->auto_index;
    (void) conf;

}








