#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <iostream>
# include <map>
# include <string>
# include <cstdlib>
# include <stddef.h>
# include <sstream>
# include <sys/time.h>
# include <unistd.h>
# include "../formats/files.hpp"
# include "../request/request.hpp"
# include "../cgi/CgiParser.hpp"
# include "../cgi/CGI.hpp"

# define EVT_INITIALIZE			1
# define EVT_CREATE_BODY			2
# define EVT_CREATE_HEADERS		3
# define EVT_SEND_RESPONSE_LINE	4
# define EVT_SEND_RESPONSE_HEADERS	5
# define EVT_SEND_RESPONSE_BODY	6

class Response {

	public:
		Response(Request *request, Descriptors *descriptors);
		~Response();

		/* Getters */
		std::string					getHost(void);
		std::string					getPort(void);
		int							getStatus(void);
		std::string					getContentLength(void);
		std::string					getStatusMessage(void);
		std::string					getMethod(void);
		std::string					getPath(void);
		std::string					getParameters(void);
		std::string					getURI(void);
		Config::location_type			getLocation(void);

		/* Methods */
		int							execute(void);
		void							createHeaders(void);
		int							createBody(void);
		int							readCGI(STRBinary & packet);

		int							readResponse(STRBinary & packet);

	private:
		Request						*_request;
		CGI							*_cgi;
		CgiParser						*_cgi_parser;
		Config::configuration_type		_server;
		std::string					_path;
		std::vector<std::string>			_directory_list;
		bool							_server_found;
		DIR							*dir;
		std::string					_body_filename;
		bool							_body_write;
		bool							_body_read;
		int							_body_fd;
		int							_status;
		int							_event;
		int							_autoIndex;
		Descriptors					*_descriptors;
		Config::location_type 			_location;
		std::map<std::string, std::string> _headers;

		Response(void);
		Response						&operator=(const Response & src);

		/* Getters */
		Descriptors::poll_type			getPoll(void);
		std::string					getUrl(std::string dirent, bool isFolder);
		int							getListOfDirectories(const char *path, STRBinary & packet);
		std::string					getPathAfterReplacingLocationByRoot(bool index = true);

		/* Methods */
		void							initialize(void);
		int							read(STRBinary & value);
		int							write(STRBinary & value);
		int							createErrorPages(std::string path, STRBinary & packet);

		std::string					findDate(void);
		std::string					findContentType(std::string path);

		void 						deleteMethod(void);
		void							postMethod(void);

		void							checkPath(void);
		void							checkRequestBody(void);
};

#endif