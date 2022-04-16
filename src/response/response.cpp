#include "response.hpp"

Response::Response(Request *request) {
	this->_request = request;
	this->_request->createTemporary("body");
	this->_request->createTemporary("response");
}

Response::~Response() {
	this->_request->closeTemporary("body");
	this->_request->closeTemporary("response");
}

std::string			Response::getHost(void)
{ return this->_request->getHost(); }

std::string			Response::getPort(void)
{ return this->_request->getPort(); }

std::string			Response::getMethod(void)
{ return this->_request->getMethod(); }

std::string			Response::getPath(void)
{ return this->_request->getPath(); }


void					Response::execute(void) {
	this->_status = this->_request->getStatus();

	Config::location_type location;
	if (this->_status < STATUS_BAD_REQUEST) {
		try {
			this->_server = this->_request->selectServer();
		} catch(const Config::ServerNotFoundException & e){
			Message::debug("Server wasn't found: handling error\n");
			this->_status = STATUS_BAD_REQUEST;
		}

		this->_path = this->_request->getPath();
		try {
			location = this->_request->selectLocation(this->_server);
			this->_location = location;
			this->_autoIndex = this->_location->auto_index;
		} catch(const Config::LocationNotFoundException& e) {
			this->_status = STATUS_NOT_FOUND;
		}
	}

	if (this->_status < STATUS_BAD_REQUEST)
		checkPath();

	if (this->_request->getMethod().compare("DELETE") == 0 && this->_status == STATUS_OK)
		deleteMethod();
}

std::string			Response::findDate(void) {
	char				buffer[80];
	struct timeval		tv;

	gettimeofday(&tv, NULL);
	strftime(buffer, 80, "%a, %d %b %Y %H:%M:%S GMT", gmtime(&tv.tv_sec));
	return (std::string(buffer));
}

void					Response::createHeaders(void) {
	size_t	body_length = this->_request->sizeTemporary("body");

	if (this->_status < STATUS_INTERNAL_SERVER_ERROR)
		this->_headers["server"] = this->_server->server_name;

	this->_headers["date"] = findDate();
	this->_headers["content-length"] = intToStr(body_length + 2);
	this->_headers["content-location"] = this->_path;
	this->_headers["content-type"] = findContentType();
}

std::string			Response::findContentType(void)
{
	std::string type;
	type = this->_path.substr(this->_path.rfind(".") + 1, this->_path.size() - this->_path.rfind("."));
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
	
	return("text/html"); //normalment text/plain mais je veux tester la page d'erreur 404 --> to check
}

void		Response::createBody(void) {
	Config::location_type	loc;

	if (this->_status == STATUS_NOT_FOUND)
		this->_request->appendTemporary("body", createErrorPages(this->_server->error_page[STATUS_NOT_FOUND]));
	else if (this->_status == STATUS_NOT_ALLOWED)
		this->_request->appendTemporary("body", createErrorPages(this->_server->error_page[STATUS_NOT_ALLOWED]));
	else if (this->_status == STATUS_INTERNAL_SERVER_ERROR)
		this->_request->appendTemporary("body", createErrorPages(this->_server->error_page[STATUS_INTERNAL_SERVER_ERROR]));
	else if (this->_status == STATUS_BAD_REQUEST)
		this->_request->appendTemporary("body", createErrorPages(this->_server->error_page[STATUS_BAD_REQUEST]));
	else if (this->_status == STATUS_REQUEST_ENTITY_TOO_LARGE)
		this->_request->appendTemporary("body", createErrorPages(this->_server->error_page[STATUS_REQUEST_ENTITY_TOO_LARGE]));
	else if (this->_status == STATUS_FORBIDDEN)
		this->_request->appendTemporary("body", createErrorPages(this->_server->error_page[STATUS_FORBIDDEN]));
	else if (this->_status == STATUS_OK) {
		try {
			loc = this->_request->selectLocation(this->_server);
		} catch(const Config::LocationNotFoundException& e) {
			this->_status = STATUS_NOT_FOUND;
			this->_request->appendTemporary("body", createErrorPages(this->_server->error_page[STATUS_NOT_FOUND]));
		}

		if (loc->root.size() > 0) {
			std::string new_p = getPathAfterReplacingLocationByRoot();
			if (isFiley(new_p) == 1)
				this->_request->appendTemporary("body", createErrorPages(new_p));
			else if (isFiley(new_p) == 2) {
				std::vector<std::string>::iterator it ;
				for (it = loc->index.begin() ; it != loc->index.end() ; it++){
					std::string path = loc->root + "/" + (*it);
					if (isFiley(path) == 1) {
						this->_request->appendTemporary("body", createErrorPages(path));
						break;
					}
				}
				if (it == loc->index.end() && this->_autoIndex) {
					this->_request->appendTemporary("body", getListOfDirectories(new_p.c_str()));
				}
			}
			else {
				this->_status = STATUS_FORBIDDEN;
				this->_request->appendTemporary("body", createErrorPages(this->_server->error_page[STATUS_FORBIDDEN]));
			}
		}
		else {
			this->_status = STATUS_NOT_FOUND;
			this->_request->appendTemporary("body", createErrorPages(this->_server->error_page[STATUS_NOT_FOUND]));
		}
	}
}

int			Response::getStatus(void)
{ return this->_status; }

std::string	Response::getStatusMessage(void) {
	if (this->_status == STATUS_OK)
		return "OK";

	if (this->_status == STATUS_NOT_FOUND)
		return "NOT FOUND";

	if (this->_status == STATUS_INTERNAL_SERVER_ERROR)
		return "INTERNEL SERVER ERROR";

	if (this->_status == STATUS_CREATED)
		return "STATUS CREATED";

	if (this->_status == STATUS_MOVED_PERMANENTLY)
		return "MOVED PERMANENTLY";

	if (this->_status == STATUS_NO_CONTENT)
		return "NO CONTENT";

	if (this->_status == STATUS_REQUEST_ENTITY_TOO_LARGE)
		return "REQUEST ENTITY TOO LARGE";

	if (this->_status == STATUS_PARTIAL_CONTENT)
		return "PARTIAL CONTENT";

	if (this->_status == STATUS_NOT_ALLOWED)
		return "NOT ALLOWED";

	if (this->_status == STATUS_FORBIDDEN)
		return "FORBIDDEN";

	return "";
}

void		Response::createResponse(void) {
	this->_request->appendTemporary("response", "HTTP/1.1 " + intToStr(this->_status) + " " + getStatusMessage() + "\r\n");
	for (std::map<std::string, std::string>::iterator it = _headers.begin(); it != _headers.end(); it++){
		this->_request->appendTemporary("response", it->first);
		this->_request->appendTemporary("response", ": ");
		this->_request->appendTemporary("response", it->second);
		this->_request->appendTemporary("response", CRLF);
	}
	this->_request->appendTemporary("response", D_CRLF);

	std::string line;
	this->_request->resetCursorTemporary("body");
	while (this->_request->readTemporary("body", line) != 0) {
		this->_request->appendTemporary("response", line);
	}

	this->_request->appendTemporary("response", CRLF);
	this->_request->resetCursorTemporary("response");
}

std::string	Response::getListOfDirectories(const char *path) {
	DIR *dir = opendir(path);

	std::string html = "<!DOCTYPE html>\n<html>\n<head>\n\
			<title>" + this->_request->getPath() + "</title>\n\
	</head>\n<body>\n<h1>Index of "+ this->_request->getPath() +"</h1>\n<p>\n";

	if (dir == NULL) {
		std::cerr << RED << "Error: could not open " << this->_request->getPath() << RESET << std::endl;
		return "";
	}

	std::vector<std::string> v;
	std::string p = getPathAfterReplacingLocationByRoot();
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

std::string	Response::getUrl(std::string dirent, bool isFolder) {
	std::string folder = "&#128193;";
	std::string file = "&#128462;";
	std::string image = file;

	if (isFolder)
		image = folder;

	std::string tmp = this->_request->getPath();
	if (tmp[tmp.size() - 1] != '/')
		tmp.append("/");

	std::string link_coln = "<td><a href=\"http://" + this->_request->getHost() + ":" + this->_request->getPort() + tmp + dirent + "\">" + dirent + "</a></td>";
	std::string line = "<tr><td>"+ image + "</td>" + link_coln +"</tr>";

	return line;
}


std::string	Response::getPathAfterReplacingLocationByRoot(void) {
	if (this->_status == STATUS_NOT_FOUND)
		return "";

	std::string loc_p = this->_location->location;
	std::string p = this->_request->getPath();

	size_t i;
	i = p.find(loc_p);

	if (i != std::string::npos) {
		if (loc_p.compare("/") != 0)
			p.erase(i, loc_p.size());
		p.insert(i, this->_location->root);
		return(p);
	}

	return "";
}

/*
void			Response::createCgiResponse(CgiParser p) {
	this->_status = p.getStatusMessage();
	create_headers(p.getBody().size());
	//append cgi headers if does not exist in _headers or replace the old value by the new one
	std::map<std::string, std::string> header = p.getHeaders();
		for (std::map<std::string, std::string>::iterator itCgi = header.begin(); itCgi != header.end(); itCgi++){
		this->_headers[itCgi->first] = itCgi->second;
	}
	this->_response.append("HTTP/1.1 ");
	this->_response.append(intToStr(p.getStatusMessage()));
	this->_response.append(" ");
	this->_response.append(getStat());
	this->_response.append(CRLF);
	for (std::map<std::string, std::string>::iterator it = _headers.begin(); it != _headers.end(); it++){
		this->_response.append(it->first);
		this->_response.append(" : ");
		this->_response.append(it->second);
		this->_response.append(CRLF);
	}
	this->_response.append(D_CRLF);
	this->_response.append(p.getBody());
	this->_response.append(CRLF);
}
*/

void		Response::deleteMethod(void) {
	std::string p = this->getPathAfterReplacingLocationByRoot();
	if (isFiley(p) == 1) {
		if (remove(p.c_str()) == 0)
			this->_status = STATUS_NO_CONTENT;
		else
			this->_status = STATUS_FORBIDDEN;
	}
	else
		this->_status = STATUS_NOT_FOUND;
}

void			Response::checkPath(void) {
	std::string	p = getPathAfterReplacingLocationByRoot();
	char			actualPath[PATH_MAX + 1];

	realpath(&p[0], actualPath);
	std::string	pp(actualPath);
	size_t		i = pp.find(this->_location->root, 0);
	if (i == std::string::npos) {
		this->_status = STATUS_BAD_REQUEST;
	}
}

std::string	Response::createErrorPages(std::string path) {
	std::string html = readHtmlFile(path);

	if (html.compare("") == 0) {
		if (this->_status == STATUS_NOT_FOUND)
			return "<!DOCTYPE html><html><title>404</title><body>404 NOT FOUND</body></html>";
		else if (this->_status == STATUS_INTERNAL_SERVER_ERROR)
			return "<!DOCTYPE html><html><title>500</title><body>500 INTERNAL SERVER ERROR</body></html>";
		else if (this->_status == STATUS_BAD_REQUEST)
			return "<!DOCTYPE html><html><title>400</title><body>400 BAD REQUEST</body></html>";
		else if (this->_status == STATUS_FORBIDDEN)
			return "<!DOCTYPE html><html><title>403</title><body>403 FORBIDDEN</body></html>";
		else if (this->_status == STATUS_REQUEST_ENTITY_TOO_LARGE)
			return "<!DOCTYPE html><html><title>413</title><body>413 ENTITY_TOO_LARGE</body></html>";
		else if (this->_status == STATUS_NOT_ALLOWED)
			return "<!DOCTYPE html><html><title>405</title><body>405 NOT ALLOWED</body></html>";
	}

	return html;
}

Config::location_type	Response::getLocation(void) {
	return(this->_location);
}