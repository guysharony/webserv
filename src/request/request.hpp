#ifndef REQUEST_HPP
# define REQUEST_HPP
# include <iostream>
# include <map>
# include <string>
# include <stddef.h>
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
        std::string                         _port;
        std::string                         _host;
		Config						        _config;
        void                                 firstLineParsing(std::string request_buffer);
        std::string		                     getNextLine(std::string str, size_t *i);
        size_t                               headerParsing(std::string request_buffer);
        void                                 parsePathAndVersion(std::string line);
        void                                 checkMethod();
        void                                 checkVersion();
        void                                 checkBody(Config::configuration_struct &server);
        void                                 checkPort();
        void                                 request_clear();
        Config::configuration_struct        &selectServer();
        Config::location_struct             &selectLocation(Config::configuration_struct &server);


    public:
        request(void);
        request(Config& conf);
        ~request(void);
        request &operator=(request const &rhs);
        request(request const &src);
        std::string getMethod(void);
        std::string getVersion(void);
        std::string getPath(void);
        std::string getHost(void);
        std::string getPort(void);
        int getRet(void);
        const std::map<std::string, std::string>& getHeader() const;
        std::string getBody(void);
        void parseRequest(std::string request_buffer);
        void displayAllLocations(void);
        


};
std::ostream&		operator<<(std::ostream& os, request& re);
std::string trim2(const std::string& str);
int	ft_atoi(const char *nptr);
int	ft_isalpha(const char * str);
void print_buffer(std::string buffer, size_t max_size, std::string color);

#endif