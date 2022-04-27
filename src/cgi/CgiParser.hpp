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
		int									getEvent(void);
		int									getEnd(void);

		/* Setters */
		void									setStatus(int status);
		void									setEvent(int value);

		/* Methods */
		void									append(STRBinary packet);
		void									parseCgiResponse(void);

	private:
		Request								*_request;
		int									_status;
		std::map<std::string, std::string>			_headers;
		STRBinary								_temp;
		STRBinary								_current;
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

		int									checkHeaders(void);
		int									checkHeader(STRBinary source, std::string & key, std::string & value);
};




#endif