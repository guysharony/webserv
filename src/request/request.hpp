#ifndef REQUEST_HPP
# define REQUEST_HPP
# include <iostream>
# include <map>
# include <algorithm>
# include <sstream>
# include <string>
# include <stddef.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <unistd.h>
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
        int                                 _timeout;
        void                                 firstLineParsing(std::string request_buffer);
        std::string		                     getNextLine(std::string str, size_t *i);
        size_t                               headerParsing(std::string request_buffer);
        void                                 parsePathAndVersion(std::string line);
        void                                 checkMethod();
        void                                 checkVersion();
        void                                 checkBody(Config::configuration_struct &server);
        void                                 checkPort();
        void                                 checkTimeout();
        void                                 request_clear();
        bool                                 checkMethodBylocation(std::vector<int> methosds_type);
        int                                  convertMethodToValue(std::string method);


      


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
        int getTimeout(void);
        int getRet(void);
        const std::map<std::string, std::string>& getHeader() const;
        std::string getBody(void);
        void parseRequest(std::string request_buffer);
        void displayAllLocations(void);
        Config::configuration_struct        &selectServer();
        Config::location_type             selectLocation(Config::configuration_struct &server);
        void                        setRet(int code);
        


};
std::ostream&		operator<<(std::ostream& os, request& re);
std::string trim2(const std::string& str);
int	ft_atoi(const char *nptr);
int	ft_isalpha(const char * str);
void print_buffer(std::string buffer, size_t max_size, std::string color);
std::string	intToStr(int a);
std::string		readHtmlFile(std::string path);
int existingDir(std::string path);
int		isFiley(std::string path);


#endif