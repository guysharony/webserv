#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <iostream>
# include <map>
# include <algorithm>
# include <sstream>
# include <string>
# include <limits.h>
# include <stdlib.h>
# include <stddef.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <unistd.h>
# include "../../include/constants.hpp"
# include "../temporary/temporary.hpp"
# include "../config/config.hpp"

class Request {

	public:
		Request(Config *config, Descriptors *descriptors);
		~Request();

		/* Getters */
		std::string							getMethod(void);
		std::string							getVersion(void);
		std::string							getPath(void);
		std::string							getReferer(void);
		std::string							getUserAgent(void);
		std::string							getParameters(void);
		std::string							getURI(void);
		std::string							getHost(void);
		std::string							getPort(void);
		Config								*getConfig(void);
		int									getClose(void);
		int									getTimeout(void);
		int									getStatus(void);
		const std::map<std::string, std::string>&	getHeader(void) const;
		int									getEvent(void);
		int									getEnd(void);
		int									getLine(void);
		int									getConnection(void);

		/* Setters */
		void									setStatus(int status);
		void									setEvent(int event);
		void									setClose(int value);
		void									setEnd(int value);
		void									setConnection(int connection);
		void									setDescriptors(Descriptors *descriptors);

		/* Temporary */
		int									createTemporary(std::string const & filename);
		int									fdTemporary(std::string const & filename);
		int									readTemporary(std::string const & filename, STRBinary &packet);
		int									appendTemporary(std::string const & filename, std::string packet);
		int									appendTemporary(std::string const & filename, STRBinary const & packet);
		int									eventTemporary(std::string const & filename, short event);
		int									displayTemporary(std::string const & filename);
		ssize_t								sizeTemporary(std::string const & filename);
		int									resetCursorTemporary(std::string const & filename);
		int									closeTemporary(std::string const & filename);
		void									parseRequest(void);

		/* Methods */
		void									execute(void);

		void									append(std::vector<char> & value);
		void									displayAllLocations(void);
		Config::configuration_type				selectServer(void);
		Config::location_type					selectLocation(Config::configuration_type server);
		bool									isCgi(Config::configuration_type server);

	private:
		std::string							_method;
		std::string							_version;
		std::map<std::string, std::string>			_header;
		int									_status;
		std::string							_path;
		std::string							_parameters;
		std::string							_port;
		std::string							_host;
		STRBinary								_temp;
		STRBinary								_current;
		Config								*_config;
		Temporary								_temporary;
		int									_encoding;
		ssize_t								_content_length;
		ssize_t								_body_size;
		ssize_t								_chunk_size;
		int									_close;
		int									_connection;
		bool									_chunked;
		int									_event;
		int									_timeout;
		int									_end;

		Request(void);
		Request(Request const & src);
		Request	&operator=(Request const &rhs);

		int									firstLineParsing(void);
		std::string							getNextLine(std::string str, size_t *i);
		size_t								headerParsing(std::string request_buffer);
		int									checkMethod(std::string & source, std::string & dst);
		int									checkPath(std::string & source, std::string & path, std::string & parameters);
		int									checkVersion(std::string & source, std::string & dst);
		void									checkBody(Config::configuration_type server);
		int									checkHeaders(void);
		int									checkHeader(std::string source, std::string & key, std::string & value);
		void									checkPort(void);
		void									checkTimeout(void);
		bool									checkMethodBylocation(std::vector<int> methosds_type);
		int									convertMethodToValue(std::string method);
};

std::ostream&		operator<<(std::ostream& os, Request& re);
std::string 		trim2(const std::string& str);
int				ft_atoi(const char *nptr);
int				ft_isalpha(const char * str);
void 			print_buffer(std::string buffer, size_t max_size, std::string color);
std::string		intToStr(int a);
std::string		readHtmlFile(std::string path);
int				existingDir(std::string path);
int				isFiley(std::string path);

#endif