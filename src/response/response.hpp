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



class response {
public:
	response(void);
	response(const response & src);
	response(request & request);
	~response(void);
	response & operator=(const response & src);

	std::string		getResponse(void);
	void		createResponse();



private:
	request						_req;
	Config::configuration_struct    _server;
	std::string					_response;
	std::string					_path;
	int							_codeDeRetour;
    int                        	_autoIndex;
	std::map<std::string, std::string> _headers;
	void 	create_headers(int body_length);
	std::string	findDate();
	std::string	findContentType();
	std::string createBody();
	std::string		getStat();



};

#endif