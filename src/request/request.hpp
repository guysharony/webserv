#ifndef REQUEST_HPP
# define REQUEST_HPP
# include <iostream>
# include <map>
# include <string>

class request{
    private:
        std::string                         _method;
        std::string                         _version;
        std::map<std::string, std::string>  _header;
        std::string                         _body;
        int                                 _ret;
        std::string                         _path;
        int                                 _port;
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
        ~request(void);
        std::string getMethod();
        std::string getVersion();
        std::string getPath();
        int getPort();
        const std::map<std::string, std::string>& getHeader() const;
        std::string getBody();
        void parseRequest(std::string request_buffer);
        


};
std::ostream&		operator<<(std::ostream& os, request& re);
std::string trim(const std::string& str);
int	ft_atoi(const char *nptr);
int	ft_isalpha(const char * str);






#endif