#include "CgiParser.hpp"

CgiParser::CgiParser(Request *request)
:
	_request(request),
	_status(STATUS_OK),
	_temp(""),
	_current(""),
	_encoding(NONE),
	_event(EVT_REQUEST_HEADERS)
{ this->_headers.clear(); }

CgiParser::~CgiParser()
{ }

CgiParser &CgiParser::operator=(CgiParser const & rhs)
{
	if (this != &rhs) {
		this->_request = rhs._request;
		this->_status = rhs._status;
		this->_temp = rhs._temp;
		this->_current = rhs._current;
		this->_encoding = rhs._encoding;
		this->_event = rhs._event;
		this->_headers = rhs._headers;
	}

	return (*this);
}

CgiParser::CgiParser(CgiParser const & src)
{ *this = src; }


/* Getters */
int									CgiParser::getStatus()
{ return( this->_status); }

const std::map<std::string, std::string>	&CgiParser::getHeaders() const
{ return(this->_headers); }

int									CgiParser::getEvent(void)
{ return (this->_event); }

int									CgiParser::getLine(void) {
	this->_current.clear();

	std::size_t	end;

	end = this->_temp.find(CRLF);
	if (end != std::string::npos) {
		this->_current = this->_temp.substr(0, end);
		this->_temp = this->_temp.substr(end + 2);
		return (2);
	}

	if (this->getEvent() < EVT_REQUEST_BODY)
		return (0);

	if (!this->_temp.length() || (this->_temp[this->_temp.length() - 1] == '\r')) {
		if (this->_temp.length()) {
			this->_current = this->_temp.substr(0, this->_temp.length() - 1);
			this->_temp = this->_temp.substr(this->_temp.length() - 1);
		}

		return (0);
	}

	this->_current = this->_temp;
	this->_temp.clear();

	return (1);
}


/* Setters */
void			CgiParser::setStatus(int status)
{ this->_status = status; }

void			CgiParser::setEvent(int value)
{ this->_event = value; }


/* Methods */
void			CgiParser::append(STRBinary packet)
{ this->_temp.append(packet); }

int			CgiParser::checkHeaders(void)
{
	std::string key;
	std::string value;

	if (this->checkHeader(this->_current, key, value)) {
		if (!key.compare("status")) {
			this->setStatus(atoi(value.c_str()));
		} else {
			this->_headers[key] = value;
		}
	}

	return 1;
}

int			CgiParser::checkHeader(STRBinary source, std::string & key, std::string & value)
{
	size_t	pos = source.find(":");

	std::string tmp_key;
	std::string tmp_value;

	if (pos != std::string::npos)
	{
		tmp_key = source.substr(0, pos).str();
		tmp_value = source.substr(pos + 1).str();

		if (!isTchar(tmp_key))
			return (0);

		trim(tmp_value);

		key = toLowercase(tmp_key);
		value = tmp_value;

		return (1);
	}

	return (0);
}

void			CgiParser::parseCgiResponse(void) {
	int		res;

	while ((res = this->getLine()) > 0) {
		if (this->getEvent() == EVT_REQUEST_HEADERS) {
			if (!this->_current.length()) {
				this->setEvent(EVT_REQUEST_BODY);
				continue;
			}

			if (!this->checkHeaders()) {
				this->setStatus(STATUS_BAD_REQUEST);
				continue;
			}

		} else if (this->getEvent() == EVT_REQUEST_BODY) {
			if (!this->_current.length()) {
				this->setStatus(STATUS_BAD_REQUEST);
				continue;
			}

			this->_request->appendTemporary("body", this->_current);
		}
	}
}