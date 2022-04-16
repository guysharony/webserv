#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <iostream>
# include <map>
# include <string>
# include <cstdlib>
# include <stddef.h>
# include <sstream>
# include <sys/time.h>
# include "../request/request.hpp"
// # include "../cgi/CgiParser.hpp"

class Response {

	public:
		Response(Request *request);
		~Response();

		/* Getters */
		std::string					getHost(void);
		std::string					getPort(void);
		int							getStatus(void);
		std::string					getStatusMessage(void);
		std::string					getMethod(void);
		std::string					getPath(void);
		Config::location_type			getLocation(void);

		/* Methods */
		void							execute(void);
		void							createResponse(void);
		void							createHeaders(void);
		void							createBody(void);
		//void						createCgiResponse(CgiParser p);

	private:
		Request						*_request;
		Config::configuration_type		_server;
		std::string					_path;
		int							_status;
		int							_autoIndex;
		Config::location_type 			_location;
		std::map<std::string, std::string> _headers;

		Response(void);
		Response						&operator=(const Response & src);

		/* Getters */
		std::string					getUrl(std::string dirent, bool isFolder);
		std::string					getListOfDirectories(const char *path);
		std::string					getPathAfterReplacingLocationByRoot(void);

		/* Methods */
		std::string					createErrorPages(std::string path);

		std::string					findDate(void);
		std::string					findContentType(void);

		void 						deleteMethod(void);

		void							checkPath(void);
};

#endif