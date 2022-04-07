# ifndef CGIRESPONSE_HPP
# define CGIRESPONSE_HPP

# include <iostream>
# include "../../include/constants.hpp"
# include <map>
# include "../request/request.hpp"

class CgiResponse
{
private:
    std::map<std::string, std::string> _headers;
    std::string                        _body;
    int                                _status;
    size_t parseHeaders(std::string buffer);
    std::string getNextLine(std::string str, size_t *i);
    void  parseStat();


public:
    CgiResponse();
    ~CgiResponse();
	CgiResponse(CgiResponse const & src);
	CgiResponse &operator=(CgiResponse const & rhs);
    void parseCgiBuffer(std::string buffer);
    
};




#endif