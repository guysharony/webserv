#ifndef REQUEST_HPP
# define REQUEST_HPP
# include <iostream>
# include <map>
# include <string>
# include "../../include/constants.hpp"
# include "../config/config.hpp"

class request{
    private:
        std::string                         _method;
        std::string                         _version;
        std::map<std::string, std::string>  _header;
        std::string                         _body;
        int                                 _ret;
        std::string                         _path;
        int                                 _port;
		Config						        _config;
        void                                 firstLineParsing(std::string request_buffer);
        std::string		                     getNextLine(std::string str, size_t *i);
        size_t                               headerParsing(std::string request_buffer);
        void                                 parsePathAndVersion(std::string line);
        void                                 checkMethod();
        void                                 checkVersion();
        void                                 checkPort();
        void                                 request_clear();

    public:
        request(void);
        request(Config& conf);
        ~request(void);
        request &operator=(request const &rhs);
        request(request const &src);
        std::string getMethod(void);
        std::string getVersion(void);
        std::string getPath(void);
        int getPort(void);
        int getRet(void);
        const std::map<std::string, std::string>& getHeader() const;
        std::string getBody(void);
        void parseRequest(std::string request_buffer);
        void displayAllLocations(void);
        


};
std::ostream&		operator<<(std::ostream& os, request& re);
std::string trim(const std::string& str);
int	ft_atoi(const char *nptr);
int	ft_isalpha(const char * str);

#endif