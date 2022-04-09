# ifndef CGIPARSER_HPP
# define CGIPARSER_HPP

# include <iostream>
# include "../../include/constants.hpp"
# include <map>
# include "../request/request.hpp"

class CgiParser
{
private:
    std::map<std::string, std::string> _headers;
    std::string                        _body;
    int                                _status;
    size_t parseHeaders(std::string buffer);
    std::string getNextLine(std::string str, size_t *i);
    void  parseStat();


public:
    CgiParser();
    ~CgiParser();
	CgiParser(CgiParser const & src);
	CgiParser &operator=(CgiParser const & rhs);
    void parseCgiBuffer(std::string buffer);
    int getStatus();
    const std::map<std::string, std::string> &getHeaders() const;
    std::string getBody();

    
};




#endif