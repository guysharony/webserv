#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <iostream>
# include <map>
# include <string>
# include <stddef.h>
# include "../../include/constants.hpp"
# include "../config/config.hpp"
# include "../request/request.hpp"



class response {
public:
	response(void);
	response(const response & src);
	response(request & request, Config& conf);
	~response(void);
	response & operator=(const response & src);

	std::string		getResponse(void);


private:
	std::string					_response;
	std::string					_path;
	int							_codeDeRetour;
    int                        _autoIndex;
};

#endif