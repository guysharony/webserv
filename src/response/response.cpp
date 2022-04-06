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
	try{
		this->_server = request.selectServer();
	}
	catch(const Config::ServerNotFoundException & e){
		Message::debug("Server wasn't found: handling error\n");
        this->_codeDeRetour = STATUS_INTERNAL_SERVER_ERROR;
	}
    this->_path = request.getPath();
    this->_autoIndex = request.selectLocation(_server)->auto_index;

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
    Config::location_type loc = _req.selectLocation(_server);
    if (_codeDeRetour == STATUS_NOT_FOUND)
        tmp.append(_unique_id, readHtmlFile(_server.error_page[404]));
    else if (_codeDeRetour == STATUS_OK){
        if (loc->root.size() > 0){
            std::string new_p = getPathAfterreplacinglocationByRoot();
            if (isFiley(new_p) == 1)
                tmp.append(_unique_id,readHtmlFile(new_p));
            else if (isFiley(new_p) == 2){
                std::vector<std::string>::iterator it ;
                for (it = loc->index.begin() ; it != loc->index.end() ; it++){
                     std::string path = loc->root + "/" + (*it);
                    if (isFiley(path) == 1){
                        tmp.append(_unique_id,readHtmlFile(path));
                        break;
                    }
                }
                if(it == loc->index.end() && this->_autoIndex)
                        tmp.append(_unique_id,getListOfDirectories(new_p.c_str()));
            else{
                _codeDeRetour = STATUS_FORBIDDEN;
                tmp.append(_unique_id, readHtmlFile(_server.error_page[403]));}
            }
        else{
            _codeDeRetour = STATUS_NOT_FOUND;
            tmp.append(_unique_id, readHtmlFile(_server.error_page[404]));}
        }
    }
    else if (_codeDeRetour == STATUS_NOT_ALLOWED)
        tmp.append(_unique_id, readHtmlFile(_server.error_page[405]));
    else if (_codeDeRetour == STATUS_INTERNAL_SERVER_ERROR)
        tmp.append(_unique_id, readHtmlFile(_server.error_page[500]));
    else if (_codeDeRetour == STATUS_BAD_REQUEST)
        tmp.append(_unique_id, readHtmlFile(_server.error_page[400]));
    else if (_codeDeRetour == STATUS_REQUEST_ENTITY_TOO_LARGE)
        tmp.append(_unique_id,readHtmlFile(_server.error_page[413]));
    else if (_codeDeRetour == STATUS_FORBIDDEN)
        tmp.append(_unique_id,readHtmlFile(_server.error_page[403]));
    else
    {
        tmp.append(_unique_id, "this response is not handled yet!!");

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
    std::string folder = "iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAABI0lEQVR42mNkIBIwwgkk8B9JiBGXRqAEPy8To50pDwcnOyM3XOl/hn/X7/18xyglxMKoKsnGiMsAMVEW5t56qR4xQeYUoFZ2ZLnfv/9vYrw/TaVHmJclF8hnxuZuFnFGRhYxRpBLGKG2gMn//0EeYPjP+G6e4guQRVBZFIcwczExcKizgEUZMT35H6zhxQzp14zMjMIoKoAsVj4WBk55VgYmVoTVWMPo426FN+xCLELYrEAEJB4Dvp9V+gtRg0cVvuj9flbxH4GYxG/AtzMK/8nRCDfg62k5ygz4fFLmH9T9JHnhPywaPx6T+ssIiQFSw+A/KDExnl8tuU5UkMGPZN1A/PYDwyZGeUkWRkUZYDpgYGAi0Yx/D5/9eUcwN+JzAQgAABw0Qn/X2Z3nAAAAAElFTkSuQmCC";
    std::string file = "iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAACkElEQVR42n2T2U8TURTGv5lpbSmlqbYEJNSiPvFkTFjUqDGCmBAVRGmpFX0xiInLszGGxJj4aIzGjb64YGmhUBbf/AMYbGJEEoMPQFg0LiQmlq7TGc+9sk1DPJlk7j0353e/c849Atat0mazdRSQrTpUVYOSyyGdTiOxHH9MrgnkmbDyt/r9/ukHDx8VM5fGgnMqEqk0vv1YwtzX73g3FPwVCATq6ejjZoDqvv7IeFXtfh1dVVUkUxn8/rOM3a5S3L51k0HqNipZBdT0RwbkfQcOIjoYgWuHG8lEAoqSQyabwd6qWjjsNg68e6drKdDdfYxiPuQBIvKhw0cQi8UgiiIFK5AkCYWFhSgnIGUESaT0NA03rl9bejs8uIfiFtcAlIJ8tK4eiwsLyCpZDmFFNJlMoChel+VkCpq4BeG+fty/11VLrnEdoKHhOCYnP/GqC4IAo9GITCYDg8HAFRUUWOCq2IlodAhXOy/pAUSVGxsbMTMzwzNTSAWDsLxXAXa7HQ6nE8PDI2j3+/SAEAFOnTyBqakvFAhkswo/YKkwgCSJKCkphcVi4YA2b2seINwnNzc3YWKCOqSxT4PRYOQ3C1Q85rNvtaNsexlGRkfh9eQBekNh+UxLC+bn5/jtrANcAd3MKs+U5AjmdlcgMjBACjx6QLA3JHtaz2JsbOzfAxFEOJ0OSCTfQDAGKS938fRYF3xtXj2gJ9gr+7weLFAbVda2leIxJSwN9qgcjm0oKipCMBSG39emA1S/eNUzfuH8OczOzvLAHA0Ru9XtdvNubLSXr9/gYru/hpbv14apqfn09JOnz4rNZhP+ZykasCudl38ORQd30Ta+EV1ptVo7zOb1cd4ckEzG4/HntPzM9n8B7MUhIOXcgRgAAAAASUVORK5CYII=";
    std::string image = file;
    if (isFolder)
        image = folder;
    std::string tmp = _req.getPath();
    if (tmp[tmp.size() - 1] != '/')
        tmp.append("/");
    std::string img_coln = "<td><img src=\"data:image/jpg;base64," + image + "\"/></td>";
    std::string link_coln = "<td><a href=\"http://" + _req.getHost() + ":" + _req.getPort() + tmp + dirent + "\">" + dirent + "</a></td>";
    std::string line = "<tr>"+img_coln + link_coln +"</tr>";
    return line ;
}


std::string Response::getPathAfterreplacinglocationByRoot(){
    Config::location_type loc = _req.selectLocation(_server);
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
