#include "response.hpp"

std::string		response::getResponse(void){
	return (this->_response);
}


response & response::operator=(const response & src)
{
	_response = src._response;
	_path = src._path;
	_codeDeRetour = src._codeDeRetour;
    _autoIndex = src._autoIndex;
    _server   = src._server;
	return (*this);
}


response::response(void){}

response::response(const response & src)
{
	*this = src;
}

response::~response(void){}

response::response(request & request){
    this->_codeDeRetour = request.getRet();
    this->_response = "";
	try{
		this->_server = request.selectServer();
	}
	catch(const Config::ServerNotFoundException & e){
		Message::debug("Server wasn't found: handling error\n");
	}
    this->_path = request.getPath();
    this->_autoIndex = request.selectLocation(_server)->auto_index;

}

std::string			response::findDate()
{
	char			buffer[80];
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	strftime(buffer, 80, "%a, %d %b %Y %H:%M:%S GMT", gmtime(&tv.tv_sec));
	return (std::string(buffer));
}

void    response::create_headers(int body_length){
   
    _headers["server_name"] = _server.server_name;
    _headers["date"] = findDate();
    _headers["content-length"] = intToStr(body_length);
    _headers["content-location"] = _path;
    _headers["content-type"] = findContentType();

    
}

std::string			response::findContentType()
{
	std::string type;
	type = _path.substr(_path.rfind(".") + 1, _path.size() - _path.rfind("."));
	if (type == "html")
		return("text/html");
	else if (type == "css")
		return("text/css");
	else if (type == "js")
		return("text/javascript");
	else if (type == "jpeg" || type == "jpg")
		return("image/jpeg");
	else if (type == "png")
		return("image/png");
	else if (type == "bmp")
		return("image/bmp");
	else
		return("text/plain");
}

std::string   response::createBody(){
    if (_codeDeRetour == STATUS_NOT_FOUND){
        return (readHtmlFile(_server.error_page[404]));
    }
    return "hello there !!";
}

std::string response::getStat(){
    if (_codeDeRetour == STATUS_OK)
        return("OK");
    if (_codeDeRetour == STATUS_NOT_FOUND)
        return("NOT FOUND");
    if (_codeDeRetour == STATUS_INTERNAL_SERVER_ERROR)
        return("INTERNEL SERVER ERROR");
    if (_codeDeRetour == STATUS_CREATED)
        return("STATUS CREATED");
    if (_codeDeRetour == STATUS_MOVED_PERMANENTLY)
        return("MOVED PERMANENTLY");
    if (_codeDeRetour == STATUS_NO_CONTENT)
        return("NO CONTENT");
    if (_codeDeRetour == STATUS_REQUEST_ENTITY_TOO_LARGE)
        return("REQUEST ENTITY TOO LARGE");
    if (_codeDeRetour == STATUS_PARTIAL_CONTENT)
        return("PARTIAL CONTENT");
    if (_codeDeRetour == STATUS_NOT_ALLOWED)
        return("NOT ALLOWED");
    return "";
}

void response::createResponse(){

    std::string body = createBody();

    create_headers(body.size());
    _response.append("HTTP/1.1 ");
    _response.append(intToStr(_codeDeRetour));
    _response.append(" ");
    _response.append(getStat());
    _response.append(CRLF);
    for (std::map<std::string, std::string>::iterator it = _headers.begin(); it != _headers.end(); it++){
        _response.append(it->first);
        _response.append(" : ");
        _response.append(it->second);
        _response.append(CRLF);
    }
    _response.append(D_CRLF);
    _response.append(body);
}







