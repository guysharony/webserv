# ifndef CGIPARSER_HPP
# define CGIPARSER_HPP

# include <iostream>
# include <map>
# include "../../include/constants.hpp"
# include "../request/request.hpp"

class CgiParser {

	public:
		CgiParser(Request *request);
		CgiParser(CgiParser const & src);
		CgiParser &operator=(CgiParser const & rhs);
		~CgiParser();

		/* Getters */
		int									getStatus();
		const std::map<std::string, std::string>	&getHeaders() const;
		// std::string							getBody();
		int									getEvent(void);
		int									getEnd(void);

		/* Setters */
		void									setStatus(int status);
		void									setEvent(int value);

		/* Methods */
		void									append(std::string packet);
		void									parseCgiResponse(void);
		// void								parseCgiBuffer(std::string buffer);

	private:
		Request								*_request;
		int									_status;
		std::map<std::string, std::string>			_headers;
		std::string							_temp;
		std::string							_current;
		int									_encoding;
		ssize_t								_content_length;
		ssize_t								_body_size;
		ssize_t								_chunk_size;
		int									_connection;
		bool									_chunked;
		int									_event;
		int									_timeout;
		int									_end;

		/* Getters */
		int									getLine(void);
		//std::string							getNextLine(std::string str, size_t *i);

		//size_t								parseHeaders(std::string buffer);
		int									checkHeaders(void);
		int									checkHeader(std::string source, std::string & key, std::string & value);
		// void								parseStatus(void);
};




#endif