#include "response.hpp"

Response::Response(Request *request, Descriptors *descriptors)
:
	_request(request),
	_cgi(NULL),
	_cgi_parser(NULL),
	_directory_list(),
	_server_found(false),
	_body_read(false),
	_body_fd(-1),
	_event(EVT_INITIALIZE),
	_descriptors(descriptors)
{ }

Response::~Response() {
	if (this->_cgi) {
		delete (this->_cgi);
		this->_cgi = NULL;
	}

	if (this->_cgi_parser) {
		delete (this->_cgi_parser);
		this->_cgi_parser = NULL;
	}

	this->_request->closeTemporary("body");
}

std::string			Response::getHost(void)
{ return this->_request->getHost(); }

std::string			Response::getPort(void)
{ return this->_request->getPort(); }

std::string			Response::getMethod(void)
{ return this->_request->getMethod(); }

std::string			Response::getPath(void)
{ return this->_request->getPath(); }

std::string			Response::getParameters(void)
{ return this->_request->getParameters(); }

std::string			Response::getURI(void)
{ return this->_request->getURI(); }

Config::location_type	Response::getLocation(void)
{ return(this->_location); }


int					Response::execute(void) {
	if (this->_event == EVT_INITIALIZE) {
		this->initialize();
		this->_event = EVT_CREATE_BODY;
		return 1;
	}

	if (this->_event == EVT_CREATE_BODY) {
		if (this->createBody() > 0)
			this->_event = EVT_CREATE_HEADERS;

		return 1;
	}

	this->createHeaders();

	return 0;
}

void					Response::initialize(void) {
	this->_body_fd = -1;
	this->_body_read = false;
	this->_server = this->_request->getConfig()->configuration.end();
	this->_status = this->_request->getStatus();
	this->_request->createTemporary("body");

	Config::location_type location;
	if (this->_status < STATUS_BAD_REQUEST && this->_status != STATUS_NOT_ALLOWED) {
		try {
			this->_server = this->_request->selectServer();
		} catch(const Config::ServerNotFoundException & e) {
			Message::debug("Server wasn't found: handling error\n");
			this->_status = STATUS_BAD_REQUEST;
		}

		this->_path = this->_request->getPath();
		try {
			location = this->_request->selectLocation(this->_server);
			this->_location = location;
			this->_autoIndex = this->_location->auto_index;
		} catch(const Config::LocationNotFoundException& e) {
			this->_status = STATUS_FORBIDDEN;
		}
	}

	if (!this->_request->isCgi(this->_server)) {
		if (this->_status < STATUS_BAD_REQUEST)
			checkPath();

		if (this->_request->getMethod().compare("DELETE") == 0 && this->_status == STATUS_OK)
			deleteMethod();

		if (this->_request->getMethod().compare("POST") == 0 && this->_status == STATUS_OK) {
			postMethod();
		}
	}

	this->_server_found = (this->_server != this->_request->getConfig()->configuration.end());
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

	if (this->_status < STATUS_INTERNAL_SERVER_ERROR && this->_server_found)
		this->_headers["Server"] = this->_server->server_name;

	this->_headers["Date"] = findDate();
	this->_headers["Content-Length"] = intToStr(body_length + 2);
	this->_headers["Content-Location"] = this->_path;
	this->_headers["Content-Type"] = findContentType();

	if (this->_status == STATUS_MOVED_PERMANENTLY) {
		this->_headers["Location"] = this->_location->redirect;
	}

	this->_event = EVT_SEND_RESPONSE_LINE;
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

int		Response::createBody(void) {
	Config::location_type	location;
	STRBinary				packet;
	bool 					isCgi = this->_request->isCgi(this->_server);

	this->_status = this->_request->getStatus();
	if (isCgi && (this->_status == STATUS_OK)) {
		if (this->readCGI(packet) > 0) {
			this->_cgi_parser->append(packet);
			this->_cgi_parser->parseCgiResponse();
			return 0;
		}

		this->_status = this->_cgi_parser->getStatus();
		this->_headers = this->_cgi_parser->getHeaders();

		if (this->_cgi) {
			delete (this->_cgi);
			this->_cgi = NULL;
		}
	
		if (this->_cgi_parser) {
			delete (this->_cgi_parser);
			this->_cgi_parser = NULL;
		}
	} else {
		if (this->_status != STATUS_OK && this->_status != STATUS_CREATED) {
			if ((createErrorPages(this->_server_found ? this->_server->error_page[this->_status] : "", packet) > 0) || (this->_body_fd <= 0)) {
				this->_request->appendTemporary("body", packet);
				return (this->_body_fd <= 0);
			}

			return 1;
		}

		try {
			location = this->_request->selectLocation(this->_server);
		} catch(const Config::LocationNotFoundException& e) {
			this->_status = STATUS_NOT_FOUND;
			return 0;
		}

		if (location->root.size() > 0)
		{
			std::string new_p = getPathAfterReplacingLocationByRoot();

			if (!this->getMethod().compare("POST") && this->_body_write) {
				if (this->_request->readTemporary("request", packet) > 0) {
					this->write(packet);
					return 0;
				}

				this->_body_write = false;
				return 1;
			}

			if (location->redirect.size() > 0) {
				this->_status = STATUS_MOVED_PERMANENTLY;
				return 0;
			}

			if (isFiley(new_p) == 1)
			{
				if ((createErrorPages(new_p, packet) > 0) || (this->_body_fd <= 0)) {
					this->_request->appendTemporary("body", packet);
					return (this->_body_fd <= 0);
				}
			}
			else if (isFiley(new_p) == 2)
			{
				std::vector<std::string>::iterator it;

				for (it = location->index.begin() ; it != location->index.end() ; it++)
				{
					std::string path = location->root + "/" + (*it);
					if (isFiley(path) == 1)
					{
						if ((createErrorPages(path, packet) > 0) || (this->_body_fd <= 0)) {
							this->_request->appendTemporary("body", packet);
							return (this->_body_fd <= 0);
						}
						break;
					}
				}

				if (it == location->index.end() && this->_autoIndex) {
					if (getListOfDirectories(new_p.c_str(), packet) > 0) {
						this->_request->appendTemporary("body", packet);
						return 0;
					}
					return 1;
				}
			}
			else
			{
				this->_status = STATUS_NOT_FOUND;
				return 0;
			}
		}
		else {
			this->_status = STATUS_NOT_FOUND;
			return 0;
		}
	}

	return 1;
}

int			Response::getStatus(void)
{ return this->_status; }

std::string	Response::getStatusMessage(void) {
	if (this->_status == STATUS_OK)
		return "OK";

	if (this->_status == STATUS_CREATED)
		return "CREATED";

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

int		Response::readResponse(STRBinary & packet) {
	if (this->_event == EVT_SEND_RESPONSE_LINE) {
		this->_event = EVT_SEND_RESPONSE_HEADERS;
		packet = "HTTP/1.1 " + intToStr(this->_status) + " " + getStatusMessage() + "\r\n";
		return 1;
	}

	if (this->_event == EVT_SEND_RESPONSE_HEADERS) {
		if (this->_headers.size() > 0) {
			std::map<std::string, std::string>::iterator it = this->_headers.begin();

			packet = it->first + ": " + it->second + "\r\n" + ((this->_headers.size() == 1) ? "\r\n" : "");

			this->_headers.erase(it->first);
		}

		if (this->_headers.size() <= 0) {
			this->_event = EVT_SEND_RESPONSE_BODY;
			this->_request->resetCursorTemporary("body");
		}

		return 1;
	}

	if (this->_event == EVT_SEND_RESPONSE_BODY) {
		packet.clear();
		if (this->_request->readTemporary("body", packet) > 0)
			return 1;

		packet = CRLF;
		this->_event = EVT_INITIALIZE;
		this->_request->closeTemporary("body");
		return 1;
	}

	return 0;
}

int		Response::getListOfDirectories(const char *path, STRBinary & packet) {
	if (this->_request->sizeTemporary("body") <= 0) {
		DIR			*dir = opendir(path);
		std::string	init = "<!DOCTYPE html>\n<html>\n<head>\n\
				<title>" + this->_request->getPath() + "</title>\n\
		</head>\n<body>\n<h1>Index of "+ this->_request->getPath() +"</h1>\n<p>\n<hr><table>";

		if (dir == NULL) {
			std::cerr << RED << "Error: could not open " << this->_request->getPath() << RESET << std::endl;
			return 0;
		}

		for (struct dirent *dirent = readdir(dir); dirent; dirent = readdir(dir)) {
			this->_directory_list.push_back(dirent->d_name);
		}

		std::sort(this->_directory_list.begin(), this->_directory_list.end());
		this->_directory_list.erase(std::find(this->_directory_list.begin(), this->_directory_list.end(), "."));
		closedir(dir);

		packet = STRBinary(init);

		return 1;
	}

	std::string p = getPathAfterReplacingLocationByRoot();

	for (std::vector<std::string>::iterator it = this->_directory_list.begin(); it != this->_directory_list.end(); it++) {
		if (isFiley(p + "/" + *it) == 2) {
			packet = getUrl(*it, true) + (this->_directory_list.size() == 1 ? "</table>\n</body>\n</html>\n" : "");
			this->_directory_list.erase(it);
			return 1;
		}
	}

	for (std::vector<std::string>::iterator it = this->_directory_list.begin(); it != this->_directory_list.end(); it++) {
		if (isFiley(p + "/" + *it) == 1) {
			packet = getUrl(*it, false) + (this->_directory_list.size() == 1 ? "</table>\n</body>\n</html>\n" : "");
			this->_directory_list.erase(it);
			return 1;
		}
	}

	return 0;
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

int			Response::readCGI(STRBinary & packet) {
	if (this->_cgi == NULL) {
		this->_cgi = new CGI(this->_request);
		this->_cgi_parser = new CgiParser(this->_request);

		this->_body_fd = this->_cgi->launch_cgi(this->getPathAfterReplacingLocationByRoot());
		if (this->_body_fd == -1)
			Message::error("Internal Error: CGI execution failed"); // *** We must properly handle this error and return a response to the client

		this->_descriptors->setDescriptor(this->_body_fd, POLLIN);
		this->_descriptors->setDescriptorType(this->_body_fd, "cgi");

		return 1;
	}

	int res = this->read(packet);

	if (!res) {
		::close(this->_body_fd);
		this->_descriptors->deleteDescriptor(this->_body_fd);
		this->_body_fd = -1;
		this->_body_read = false;
	}

	return res;
}

void			Response::deleteMethod(void) {
	std::string p = this->getPathAfterReplacingLocationByRoot();
	int ret;
	ret = isFiley(p);
	if (ret == 1) {
		if (remove(p.c_str()) == 0)
			this->_status = STATUS_NO_CONTENT;
		else
			this->_status = STATUS_FORBIDDEN;
	}
	else if (ret == -1)
		this->_status = STATUS_FORBIDDEN;
	else
		this->_status = STATUS_NOT_FOUND;
}

void			Response::postMethod(void) {
	std::string p = this->getPathAfterReplacingLocationByRoot();

	this->_request->resetCursorTemporary("request");
	this->_request->eventTemporary("request", POLLIN);

	if (exists(p)) {
		if (isFile(p)) {
			this->_status = STATUS_OK;
			if ((this->_body_fd = open(p.c_str(), O_RDWR)) < 0) {
				this->_status = STATUS_NOT_ALLOWED;
				return;
			}

			this->_body_write = true;

			this->_headers["Location"] = this->_request->getPath();

			fcntl(this->_body_fd, F_SETFL, O_NONBLOCK);

			lseek(this->_body_fd, 0, SEEK_END);

			this->_descriptors->setDescriptor(this->_body_fd, POLLOUT);
			this->_descriptors->setDescriptorType(this->_body_fd, "file");
		} else if (isDirectory(p)) {
			if ((this->_body_fd = uniqueFile(p, this->_body_filename, O_CREAT | O_TRUNC | O_RDWR, S_IRWXU)) < 0) {
				this->_status = STATUS_NOT_ALLOWED;
				return;
			}

			this->_headers["Location"] = secureAddress(this->_request->getPath(), this->_body_filename);

			this->_status = STATUS_CREATED;
			this->_body_write = true;

			fcntl(this->_body_fd, F_SETFL, O_NONBLOCK);

			this->_descriptors->setDescriptor(this->_body_fd, POLLOUT);
			this->_descriptors->setDescriptorType(this->_body_fd, "file");
		}
	} else {
		this->_status = STATUS_NOT_ALLOWED;
	}
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

int		Response::createErrorPages(std::string path, STRBinary & packet) {
	if (this->_body_fd == -1) {
		if ((this->_body_fd = open(path.c_str(), O_RDONLY)) <= 0) {
			if (this->_status == STATUS_NOT_FOUND)
				packet = "<!DOCTYPE html><html><title>404</title><body>404 NOT FOUND</body></html>";
			else if (this->_status == STATUS_INTERNAL_SERVER_ERROR)
				packet = "<!DOCTYPE html><html><title>500</title><body>500 INTERNAL SERVER ERROR</body></html>";
			else if (this->_status == STATUS_BAD_REQUEST)
				packet = "<!DOCTYPE html><html><title>400</title><body>400 BAD REQUEST</body></html>";
			else if (this->_status == STATUS_FORBIDDEN)
				packet = "<!DOCTYPE html><html><title>403</title><body>403 FORBIDDEN</body></html>";
			else if (this->_status == STATUS_REQUEST_ENTITY_TOO_LARGE)
				packet = "<!DOCTYPE html><html><title>413</title><body>413 ENTITY_TOO_LARGE</body></html>";
			else if (this->_status == STATUS_NOT_ALLOWED)
				packet = "<!DOCTYPE html><html><title>405</title><body>405 NOT ALLOWED</body></html>";
			else if (this->_status == STATUS_HTTP_VERSION_NOT_SUPPORTED)
				packet = "<!DOCTYPE html><html><title>405</title><body>505 STATUS_HTTP_VERSION_NOT_SUPPORTED</body></html>";

			return 0;
		}

		fcntl(this->_body_fd, F_SETFL, O_NONBLOCK);

		this->_descriptors->setDescriptor(this->_body_fd, POLLIN);
		this->_descriptors->setDescriptorType(this->_body_fd, "file");

		packet = "";

		return 1;
	}

	int res = this->read(packet);

	if (!res) {
		::close(this->_body_fd);
		this->_descriptors->deleteDescriptor(this->_body_fd);
	}

	return res;
}

Descriptors::poll_type	Response::getPoll(void)
{
	Descriptors::poll_type	ite = this->_descriptors->descriptors.end();
	for (Descriptors::poll_type	it = this->_descriptors->descriptors.begin(); it != ite; ++it) {
		if (it->fd == this->_body_fd) {
			return it;
		}
	}

	return ite;
}

int					Response::read(STRBinary & value)
{
	Descriptors::poll_type	it;
	std::vector<char>		packet(5000);
	ssize_t				pos;

	pos = 0;

	if ((it = this->getPoll()) == this->_descriptors->descriptors.end()) {
		return -1;
	}

	value.clear();

	if (!(it->revents & POLLIN)) {
		return !this->_body_read;
	}

	pos = ::read(this->_body_fd, packet.data(), packet.size());

	if (static_cast<int>(packet.size()) > pos) {
		packet.resize(pos);
	}

	value = packet;

	this->_body_read = true;

	return (pos > 0 && value.length() > 0);
}

int					Response::write(STRBinary & value)
{
	Descriptors::poll_type	it;
	ssize_t				pos;

	if ((it = this->getPoll()) == this->_descriptors->descriptors.end()) {
		return -1;
	}

	if (!(it->revents & POLLOUT)) {
		return !this->_body_write;
	}

	pos = ::write(this->_body_fd, value.data(), value.length());

	this->_body_write = true;

	return pos > 0;
}
