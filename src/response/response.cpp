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

std::string			Response::getContentLength(void)
{ return toString(this->_request->sizeTemporary("body")); }

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
		if (this->createBody() > 0) {
			this->_event = EVT_CREATE_HEADERS;
		}

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

	if (this->_status < STATUS_BAD_REQUEST && this->_status != STATUS_NOT_ALLOWED) {
		try {
			this->_server = this->_request->selectServer();
		} catch(const Config::ServerNotFoundException & e) {
			this->_status = STATUS_BAD_REQUEST;
		}

		this->_path = this->_request->getPath();
		try {
			this->_location = this->_request->selectLocation(this->_server);
			this->checkRequestBody();
		} catch (Config::MethodNotAllowed const & e) {
			this->_status = STATUS_NOT_ALLOWED;
		} catch (Config::LocationNotFoundException const & e) {
			this->_status = STATUS_FORBIDDEN;
		}
	}

	if (!this->_request->isCgi(this->_server, getPathAfterReplacingLocationByRoot())) {
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

void					Response::checkRequestBody(void) {
	ssize_t			max_size = static_cast<ssize_t>(this->_location->client_max_body_size);
	ssize_t			current_size = this->_request->sizeTemporary("request");

	if (max_size >= 0 && current_size > max_size) {
		this->_status = STATUS_REQUEST_ENTITY_TOO_LARGE;
		return;
	}

	this->_autoIndex = this->_location->auto_index;
}

void					Response::createHeaders(void) {
	size_t	body_length = this->_request->sizeTemporary("body");

	this->_headers["Server"] = SERVER_NAME;
	this->_headers["Date"] = findDate();
	this->_headers["Content-Length"] = toString(body_length);

	if (this->_status == STATUS_MOVED_PERMANENTLY) {
		this->_headers["Location"] = this->_location->redirect;
	}

	this->_event = EVT_SEND_RESPONSE_LINE;
}

std::string			Response::findContentType(std::string path)
{
	std::string type;
	type = path.substr(path.rfind(".") + 1, path.size() - path.rfind("."));

	if (type == "html" || type == "htm" || type == "shtml")
		return "text/html";

	if (type == "css")
		return "text/css";

	if (type == "xml")
		return "text/xml";

	if (type == "git")
		return "image/gif";

	if (type == "jpeg" || type == "jpg")
		return "image/jpeg";

	if (type == "js")
		return "application/javascript";

	if (type == "atom")
		return "application/atom+xml";

	if (type == "rss")
		return "application/rss+xml";

	if (type == "mml")
		return "text/mathml";

	if (type == "txt")
		return "text/plain";

	if (type == "jad")
		return "text/vnd.sun.j2me.app-descriptor";

	if (type == "wml")
		return "text/vnd.wap.wml";

	if (type == "htc")
		return "text/x-component";

	if (type == "png")
		return "image/png";
	
	if (type == "svg" || type == "svgz")
		return "image/svg+xml";

	if (type == "tif" || type == "tiff")
		return "image/tiff";

	if (type == "wbmp")
		return "image/vnd.wap.wbmp";

	if (type == "webp")
		return "image/webp";

	if (type == "ico")
		return "image/x-icon";

	if (type == "jng")
		return "image/x-jng";

	if (type == "bmp")
		return "image/x-ms-bmp";

	if (type == "woff")
		return "font/woff";

	if (type == "woff2")
		return "font/woff2";

	if (type == "jar" || type == "war" || type == "ear")
		return "application/java-archive";

	if (type == "json")
		return "application/json";

	if (type == "hqx")
		return "application/mac-binhex40";

	if (type == "doc")
		return "application/msword";

	if (type == "pdf")
		return "application/pdf";

	if (type == "ps" || type == "eps" || type == "ai")
		return "application/postscript";

	if (type == "rtf")
		return "application/rtf";

	if (type == "m3u8")
		return "application/vnd.apple.mpegurl";

	if (type == "kml")
		return "application/vnd.google-earth.kml+xml";

	if (type == "kmz")
		return "application/vnd.google-earth.kmz";

	if (type == "xls")
		return "application/vnd.ms-excel";

	if (type == "eot")
		return "application/vnd.ms-fontobject";

	if (type == "ppt")
		return "application/vnd.ms-powerpoint";

	if (type == "odg")
		return "application/vnd.oasis.opendocument.graphics";

	if (type == "odp")
		return "application/vnd.oasis.opendocument.presentation";

	if (type == "ods")
		return "application/vnd.oasis.opendocument.spreadsheet";

	if (type == "odt")
		return "application/vnd.oasis.opendocument.text";

	if (type == "pptx")
		return "application/vnd.openxmlformats-officedocument.presentationml.presentation";

	if (type == "xlsx")
		return "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";

	if (type == "docx")
		return "application/vnd.openxmlformats-officedocument.wordprocessingml.document";

	if (type == "wmlc")
		return "application/vnd.wap.wmlc";

	if (type == "7z")
		return "application/x-7z-compressed";

	if (type == "cco")
		return "application/x-cocoa";

	if (type == "jardiff")
		return "application/x-java-archive-diff";

	if (type == "jnlp")
		return "application/x-java-jnlp-file";

	if (type == "run")
		return "application/x-makeself";

	if (type == "pl" || type == "pm")
		return "application/x-perl";

	if (type == "prc" || type == "pdb")
		return "application/x-pilot";

	if (type == "rar")
		return "application/x-rar-compressed";

	if (type == "rpm")
		return "application/x-redhat-package-manager";

	if (type == "sea")
		return "application/x-sea";

	if (type == "swf")
		return "application/x-shockwave-flash";

	if (type == "sit")
		return "application/x-stuffit";

	if (type == "tcl" || type == "tk")
		return "application/x-tcl";

	if (type == "der" || type == "pem" || type == "crt")
		return "application/x-x509-ca-cert";

	if (type == "xpi")
		return "application/x-xpinstall";

	if (type == "xhtml")
		return "application/xhtml+xml";

	if (type == "xspf")
		return "application/xspf+xml";

	if (type == "zip")
		return "application/zip";

	if (type == "bin" || type == "exe" || type == "dll")
		return "application/octet-stream";

	if (type == "deb")
		return "application/octet-stream";

	if (type == "dmg")
		return "application/octet-stream";

	if (type == "iso" || type == "img")
		return "application/octet-stream";

	if (type == "msi" || type == "msp" || type == "msm")
		return "application/octet-stream";

	if (type == "mid" || type == "midi" || type == "kar")
		return "audio/midi";

	if (type == "mp3")
		return "audio/mpeg";

	if (type == "ogg")
		return "audio/ogg";

	if (type == "m4a")
		return "audio/x-m4a";

	if (type == "ra")
		return "audio/x-realaudio";

	if (type == "3gpp" || type == "3gp")
		return "video/3gpp";

	if (type == "ts")
		return "video/mp2t";

	if (type == "mp4")
		return "video/mp4";

	if (type == "mpeg" || type == "mpg")
		return "video/mpeg";

	if (type == "mov")
		return "video/quicktime";

	if (type == "webm")
		return "video/webm";

	if (type == "flv")
		return "video/x-flv";

	if (type == "m4v")
		return "video/x-m4v";

	if (type == "mng")
		return "video/x-mng";

	if (type == "asx" || type == "asf")
		return "video/x-ms-asf";

	if (type == "wmv")
		return "video/x-ms-wmv";

	if (type == "avi")
		return "video/x-msvideo";

	return "application/octet-stream";
}

int		Response::createBody(void) {
	Config::location_type	location;
	STRBinary				packet;
	bool 				isCgi = this->_request->isCgi(this->_server, getPathAfterReplacingLocationByRoot());

	if (this->_status != STATUS_OK && this->_status != STATUS_CREATED && this->_status != STATUS_MOVED_PERMANENTLY) {
		this->_headers["Connection"] = "close";
		this->_request->setConnection(CLOSE);
		this->_headers["Content-Type"] = "text/html";
		if (this->_server_found && this->_server->error_page.count(this->_status) && isFile(this->_server->error_page[this->_status]))
			this->_headers["Content-Type"] = findContentType(this->_server->error_page[this->_status]);

		if ((createErrorPages(this->_server_found ? this->_server->error_page[this->_status] : "", packet) > 0) || (this->_body_fd <= 0)) {
			this->_request->appendTemporary("body", packet);
			return (this->_body_fd <= 0);
		}

		return 1;
	}

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

		return (this->_status == STATUS_OK || this->_status == STATUS_CREATED);
	} else {
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
				return 1;
			}

			if (isFile(new_p))
			{
				this->_headers["Content-Type"] = findContentType(new_p);

				if ((createErrorPages(new_p, packet) > 0) || (this->_body_fd <= 0)) {
					this->_request->appendTemporary("body", packet);
					return (this->_body_fd <= 0);
				}
			}
			else if (isDirectory(new_p))
			{
				std::vector<std::string>::iterator it;

				for (it = location->index.begin() ; it != location->index.end() ; it++)
				{
					std::string path = secureAddress(new_p, *it);
					if (isFile(path))
					{
						this->_headers["Content-Type"] = findContentType(path);

						if ((createErrorPages(path, packet) > 0) || (this->_body_fd <= 0)) {
							this->_request->appendTemporary("body", packet);
							return (this->_body_fd <= 0);
						}

						return (1);
					}
				}

				if (it == location->index.end() && this->_autoIndex) {
					this->_headers["Content-Type"] = "text/html";

					if (getListOfDirectories(new_p.c_str(), packet) > 0) {
						this->_request->appendTemporary("body", packet);
						return 0;
					}
					return 1;
				}

				this->_status = STATUS_FORBIDDEN;
				return 0;
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
	if (!isHttpStatus(this->_status))
		this->_status = STATUS_INTERNAL_SERVER_ERROR;

	return getHttpStatusMessage(this->_status);
}

int		Response::readResponse(STRBinary & packet) {
	if (this->_event == EVT_SEND_RESPONSE_LINE) {
		this->_event = EVT_SEND_RESPONSE_HEADERS;
		packet = "HTTP/1.1 " + toString(this->_status) + " " + getStatusMessage() + "\r\n";
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

		if (dir == NULL)
			return 0;

		for (struct dirent *dirent = readdir(dir); dirent; dirent = readdir(dir))
			this->_directory_list.push_back(dirent->d_name);

		std::sort(this->_directory_list.begin(), this->_directory_list.end());
		this->_directory_list.erase(std::find(this->_directory_list.begin(), this->_directory_list.end(), "."));
		closedir(dir);

		packet = STRBinary(init);

		return 1;
	}

	std::string p = getPathAfterReplacingLocationByRoot();

	for (std::vector<std::string>::iterator it = this->_directory_list.begin(); it != this->_directory_list.end(); it++) {
		if (isDirectory(secureAddress(p, *it))) {
			packet = getUrl(*it, true) + (this->_directory_list.size() == 1 ? "</table>\n</body>\n</html>\n" : "");
			this->_directory_list.erase(it);
			return 1;
		}
	}

	for (std::vector<std::string>::iterator it = this->_directory_list.begin(); it != this->_directory_list.end(); it++) {
		if (isFile(secureAddress(p, *it))) {
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


std::string	Response::getPathAfterReplacingLocationByRoot(bool index) {
	if (this->_status == STATUS_NOT_FOUND)
		return "";
	if (this->_status < STATUS_BAD_REQUEST){
		std::string loc_p = this->_location->location;
		std::string p = this->_request->getPath();


		size_t i;
		i = p.find(loc_p);

		if (i != std::string::npos) {
			if (loc_p.compare("/") != 0)
				p.erase(i, loc_p.size());
			p.insert(i, this->_location->root);
			if (isDirectory(p) && index) {
					std::vector<std::string>::iterator it;

					for (it = _location->index.begin() ; it != _location->index.end() ; it++)
					{
						std::string path = secureAddress(p, *it);
						if (isFile(path))
						{
							p = path;
							break;
						}
					}
			}
			return(p);
		}
	}

	return "";
}

int			Response::readCGI(STRBinary & packet) {
	if (this->_cgi == NULL) {
		this->_cgi = new CGI(this->_request);
		this->_cgi_parser = new CgiParser(this->_request);

		this->_body_fd = this->_cgi->launch_cgi(this->getPathAfterReplacingLocationByRoot());
		if (this->_body_fd == -1) {
			this->_cgi_parser->setStatus(STATUS_INTERNAL_SERVER_ERROR);
			return 0;
		}

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
	if (isFile(p)) {
		if (remove(p.c_str()) == 0)
			this->_status = STATUS_NO_CONTENT;
		else
			this->_status = STATUS_FORBIDDEN;
	}
	else if (!exists(p))
		this->_status = STATUS_FORBIDDEN;
	else
		this->_status = STATUS_NOT_FOUND;
}

void			Response::postMethod(void) {
	std::string p = this->getPathAfterReplacingLocationByRoot(false);

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

			this->_headers["Content-Location"] = this->_request->getPath();

			fcntl(this->_body_fd, F_SETFL, O_NONBLOCK);

			lseek(this->_body_fd, 0, SEEK_END);

			this->_descriptors->setDescriptor(this->_body_fd, POLLOUT);
			this->_descriptors->setDescriptorType(this->_body_fd, "file");
		} else if (isDirectory(p)) {
			if ((this->_body_fd = uniqueFile(p, this->_body_filename, O_CREAT | O_TRUNC | O_RDWR, S_IRWXU)) < 0) {
				this->_status = STATUS_NOT_ALLOWED;
				return;
			}

			this->_headers["Content-Location"] = secureAddress(this->_request->getPath(), this->_body_filename);

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
	std::string	status;
	std::string	message;

	if (this->_body_fd == -1) {
		this->_request->clearTemporary("body");
		if ((this->_body_fd = open(path.c_str(), O_RDONLY)) <= 0) {
			message = this->getStatusMessage();
			status = toString(this->_status);

			packet = "<!DOCTYPE html><html><title>" + status + "</title><body>" + status + " " + message + "</body></html>";

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

	pos = ::write(this->_body_fd, value.c_str(), value.length());

	this->_body_write = true;

	return pos > 0;
}
