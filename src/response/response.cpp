#include "response.hpp"

int 					_unique_id = rand();

std::string		Response::getResponse(void){
	return (this->_response);
}


Response & Response::operator=(const Response & src)
{
    _req = src._req;
	_response = src._response;
	_path = src._path;
	_codeDeRetour = src._codeDeRetour;
    _autoIndex = src._autoIndex;
    _server   = src._server;
	return (*this);
}


Response::Response(void){}

Response::Response(const Response & src)
{
	*this = src;
}

Response::~Response(void){}

Response::Response(Request & request){
    this->_req = request;
    this->_codeDeRetour = request.getRet();
    this->_response = "";
    if (_codeDeRetour < STATUS_BAD_REQUEST){
        try{
            this->_server = request.selectServer();
        }
        catch(const Config::ServerNotFoundException & e){
            Message::debug("Server wasn't found: handling error\n");
            this->_codeDeRetour = STATUS_BAD_REQUEST;
        }
        this->_path = request.getPath();
        try{
        this->_autoIndex = request.selectLocation(_server)->auto_index;}
        catch(const Config::LocationNotFoundException& e){
            _codeDeRetour = STATUS_NOT_FOUND;
        }
    }

}

std::string			Response::findDate()
{
	char			buffer[80];
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	strftime(buffer, 80, "%a, %d %b %Y %H:%M:%S GMT", gmtime(&tv.tv_sec));
	return (std::string(buffer));
}

void    Response::create_headers(int body_length){
   if (this->_codeDeRetour < STATUS_INTERNAL_SERVER_ERROR)
        _headers["server_name"] = _server.server_name;
    _headers["date"] = findDate();
    _headers["content-length"] = intToStr(body_length + 2);
    _headers["content-location"] = _path;
    _headers["content-type"] = findContentType();

    
}

std::string			Response::findContentType()
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
		return("text/html"); //normalment text/plain mais je veux tester la page d'erreur 404 --> to check
}

Temporary   Response::createBody(){
    Temporary tmp(_unique_id);
    tmp.create(_unique_id);
    Config::location_type loc;


    if (_codeDeRetour == STATUS_NOT_FOUND)
        tmp.append(_unique_id, createErrorPages(_server.error_page[404]));
    else if (_codeDeRetour == STATUS_NOT_ALLOWED)
        tmp.append(_unique_id, createErrorPages(_server.error_page[405]));
    else if (_codeDeRetour == STATUS_INTERNAL_SERVER_ERROR)
        tmp.append(_unique_id, createErrorPages(_server.error_page[500]));
    else if (_codeDeRetour == STATUS_BAD_REQUEST)
        tmp.append(_unique_id, createErrorPages(_server.error_page[400]));
    else if (_codeDeRetour == STATUS_REQUEST_ENTITY_TOO_LARGE)
        tmp.append(_unique_id,createErrorPages(_server.error_page[413]));
    else if (_codeDeRetour == STATUS_FORBIDDEN)
        tmp.append(_unique_id,createErrorPages(_server.error_page[403]));
    else if (_codeDeRetour == STATUS_OK){
        try{ 
            loc = _req.selectLocation(_server);
        }
        catch(const Config::LocationNotFoundException& e){
            _codeDeRetour = STATUS_NOT_FOUND;
            tmp.append(_unique_id, createErrorPages(_server.error_page[404]));
        }
        if (loc->root.size() > 0){
            std::string new_p = getPathAfterreplacinglocationByRoot();
            if (isFiley(new_p) == 1)
                tmp.append(_unique_id,createErrorPages(new_p));
            else if (isFiley(new_p) == 2){
                std::vector<std::string>::iterator it ;
                for (it = loc->index.begin() ; it != loc->index.end() ; it++){
                     std::string path = loc->root + "/" + (*it);
                    if (isFiley(path) == 1){
                        tmp.append(_unique_id,createErrorPages(path));
                        break;
                    }
                }
                if(it == loc->index.end() && this->_autoIndex)
                        tmp.append(_unique_id,getListOfDirectories(new_p.c_str()));
            else{
                _codeDeRetour = STATUS_FORBIDDEN;
                tmp.append(_unique_id, createErrorPages(_server.error_page[403]));}
            }
        else{
            _codeDeRetour = STATUS_NOT_FOUND;
            tmp.append(_unique_id, createErrorPages(_server.error_page[404]));}
        }
    }

    return(tmp);      
}

std::string Response::getStat(){
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
    if (_codeDeRetour == STATUS_FORBIDDEN)
        return("FORBIDDEN");
    return "";
}

void Response::createResponse(){
    if (_codeDeRetour < STATUS_BAD_REQUEST)
        checkPath();
    if (_req.getMethod().compare("DELETE") == 0 && _codeDeRetour == STATUS_OK)
        deleteMethod();
    Temporary body = createBody();
    create_headers(body.size(_unique_id));
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
    std::string line;
    body.cursor(_unique_id, 0);
    while (body.read(_unique_id, line) != 0){
        _response.append(line);
    }
    _response.append(CRLF);
}

std::string    Response::getListOfDirectories(const char *path) {
    DIR *dir = opendir(path);

    std::string html ="<!DOCTYPE html>\n<html>\n<head>\n\
            <title>" + _req.getPath() + "</title>\n\
    </head>\n<body>\n<h1>Index of "+ _req.getPath() +"</h1>\n<p>\n";
    if (dir == NULL) {
        std::cerr << RED << "Error: could not open " << _req.getPath() << RESET << std::endl;
        return "";
    }
    std::vector<std::string> v;
    std::string p = getPathAfterreplacinglocationByRoot();
    for (struct dirent *dirent = readdir(dir); dirent; dirent = readdir(dir)) {
        v.push_back(dirent->d_name);
    }
    std::sort(v.begin(), v.end());
    v.erase(std::find(v.begin(), v.end(), "."));
    html += "<hr>";
    html += "<table>";
    for (std::vector<std::string>::iterator it = v.begin(); it != v.end(); it++){
        if (isFiley(p+ "/" + *it) == 2)
            html += getUrl(*it, true);
    }
    for (std::vector<std::string>::iterator it = v.begin(); it != v.end(); it++){
        if (isFiley(p+ "/" + *it) == 1)
            html += getUrl(*it, false);
    }
    html +="</table>\n</body>\n</html>\n";
    closedir(dir);
    return html;
}

std::string       Response::getUrl(std::string dirent, bool isFolder) {
    std::string folder = "&#128193;";
	std::string file = "&#128462;";
    std::string image = file;
    if (isFolder)
        image = folder;
    std::string tmp = _req.getPath();
    if (tmp[tmp.size() - 1] != '/')
        tmp.append("/");
    std::string link_coln = "<td><a href=\"http://" + _req.getHost() + ":" + _req.getPort() + tmp + dirent + "\">" + dirent + "</a></td>";
	std::string line = "<tr><td>"+ image + "</td>" + link_coln +"</tr>";
    return line ;
}


std::string Response::getPathAfterreplacinglocationByRoot(){
    Config::location_type loc;
    try{
             loc = _req.selectLocation(_server);
             this->_loc = loc;
        }
    catch(const Config::LocationNotFoundException& e){
                _codeDeRetour = STATUS_NOT_FOUND;
                return "";
        }
    std::string loc_p = loc->location;
    std::string p = _req.getPath();

    size_t i;
    i = p.find(loc_p);
    if (i != std::string::npos){
        if (loc_p.compare("/") != 0)
            p.erase(i, loc_p.size());
        p.insert(i, loc->root);
        return(p);
    }
    return "";
}

void Response::createCgiResponse(CgiParser p){
    create_headers(p.getBody().size());
    //append cgi headers if does not exist in _headers or replace the old value by the new one
    std::map<std::string, std::string> header = p.getHeaders();
     for (std::map<std::string, std::string>::iterator itCgi = header.begin(); itCgi != header.end(); itCgi++){
        _headers[itCgi->first] = itCgi->second;
    } 
    _response.append("HTTP/1.1 ");
    _response.append(intToStr(p.getStatus()));
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
    _response.append(p.getBody());
    _response.append(CRLF);
}

void		Response::deleteMethod(){
    std::string p = getPathAfterreplacinglocationByRoot();
	if (isFiley(p) == 1){
		if (remove(p.c_str()) == 0)
			_codeDeRetour = STATUS_NO_CONTENT;
		else
			_codeDeRetour = STATUS_FORBIDDEN;
	}
	else
		_codeDeRetour = STATUS_NOT_FOUND;
}

void Response::checkPath(){
    std::string p = getPathAfterreplacinglocationByRoot();
	char actualPath[PATH_MAX+1];
	realpath(&p[0], actualPath);
	std::string pp(actualPath);
	size_t i = pp.find(_loc->root, 0);
	if (i == std::string::npos){
		_codeDeRetour = STATUS_BAD_REQUEST;
	}
}

std::string Response::createErrorPages(std::string path){
    std::string html = readHtmlFile(path);
    if (html.compare("") == 0){
        if (_codeDeRetour == STATUS_NOT_FOUND)
            return ("<!DOCTYPE html><html><title>404</title><body>404 NOT FOUND</body></html>");
        else if (_codeDeRetour == STATUS_INTERNAL_SERVER_ERROR)
            return ("<!DOCTYPE html><html><title>500</title><body>500 INTERNAL SERVER ERROR</body></html>");
        else if (_codeDeRetour == STATUS_BAD_REQUEST)
            return ("<!DOCTYPE html><html><title>400</title><body>400 BAD REQUEST</body></html>");
        else if (_codeDeRetour == STATUS_FORBIDDEN)
            return ("<!DOCTYPE html><html><title>403</title><body>403 FORBIDDEN</body></html>");
        else if (_codeDeRetour == STATUS_REQUEST_ENTITY_TOO_LARGE)
            return ("<!DOCTYPE html><html><title>413</title><body>413 ENTITY_TOO_LARGE</body></html>");
        else if (_codeDeRetour == STATUS_NOT_ALLOWED)
            return ("<!DOCTYPE html><html><title>405</title><body>405 NOT ALLOWED</body></html>");
    }
    return html;
}