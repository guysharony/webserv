#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <iostream>
# include <map>
# include <string>
# include <stddef.h>
# include <sstream>
# include <sys/time.h>
# include "../../include/constants.hpp"
# include "../config/config.hpp"
# include "../request/request.hpp"
# include "../formats/temporary.hpp"
#include "../cgi/CgiParser.hpp"
# include<cstdlib>



class Response {
public:
	Response(void);
	Response(const Response & src);
	Response(Request & request);
	~Response(void);
	Response & operator=(const Response & src);

	std::string		getResponse(void);
	void		createResponse();
	void createCgiResponse(CgiParser p);

private:
	Request						_req;
	Config::configuration_struct    _server;
	std::string					_response;
	std::string					_path;
	int							_codeDeRetour;
    int                        	_autoIndex;
	std::map<std::string, std::string> _headers;
	void 	create_headers(int body_length);
	std::string	findDate();
	std::string	findContentType();
	Temporary createBody();
	std::string		getStat();
	std::string     getUrl(std::string dirent, bool isFolder);
	std::string   	getListOfDirectories(const char *path);
	std::string getPathAfterreplacinglocationByRoot();
	void 	deleteMethod();
	


};

#endif