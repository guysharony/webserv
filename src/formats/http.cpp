#include "http.hpp"


int				isHttpStatus(int status)
{
	return (status == STATUS_OK
	|| status == STATUS_CREATED
	|| status == STATUS_NO_CONTENT
	|| status == STATUS_BAD_REQUEST
	|| status == STATUS_FORBIDDEN
	|| status == STATUS_NOT_FOUND
	|| status == STATUS_NOT_ALLOWED
	|| status == STATUS_NOT_IMPLEMENTED
	|| status == STATUS_MOVED_PERMANENTLY
	|| status == STATUS_REQUEST_ENTITY_TOO_LARGE
	|| status == STATUS_REQUEST_URI_TOO_LARGE
	|| status == STATUS_INTERNAL_SERVER_ERROR
	|| status == STATUS_HTTP_VERSION_NOT_SUPPORTED);
}

std::string		getHttpStatusMessage(int status)
{
	if (status == STATUS_OK) return ("OK");
	if (status == STATUS_CREATED) return ("Created");
	if (status == STATUS_NO_CONTENT) return ("No Content");
	if (status == STATUS_BAD_REQUEST) return ("Bad Request");
	if (status == STATUS_FORBIDDEN) return ("Forbidden");
	if (status == STATUS_NOT_FOUND) return ("Not Found");
	if (status == STATUS_NOT_ALLOWED) return ("Not Allowed");
	if (status == STATUS_NOT_IMPLEMENTED) return ("Not Implemented");
	if (status == STATUS_MOVED_PERMANENTLY) return ("Moved Permanently");
	if (status == STATUS_REQUEST_ENTITY_TOO_LARGE) return ("Request Entity Too Large");
	if (status == STATUS_REQUEST_URI_TOO_LARGE) return ("Request-URI Too Large");
	if (status == STATUS_INTERNAL_SERVER_ERROR) return ("Internal Server Error");
	if (status == STATUS_HTTP_VERSION_NOT_SUPPORTED) return ("Http Version Not Supported");

	return ("");
}

int		isHttpMethod(std::string value, int & method)
{
	if (!value.compare("GET")) {
		method = METHOD_GET;
		return (1);
	}

	if (!value.compare("POST")) {
		method = METHOD_POST;
		return (1);
	}

	if (!value.compare("DELETE")) {
		method = METHOD_DELETE;
		return (1);
	}

	return (0);
}

int		isHttpMethod(std::string value)
{
	if (!value.compare("GET"))
		return (1);

	if (!value.compare("POST"))
		return (1);

	if (!value.compare("DELETE"))
		return (1);

	return (0);
}

std::string 	getDate(std::string format)
{
	struct timeval 	tmp;
	struct tm 		*timeinfo;
	char				buffer[80];

	gettimeofday(&tmp, NULL);
	timeinfo = localtime(&tmp.tv_sec);

	strftime(buffer, 80, format.c_str(), timeinfo);

	return (std::string(buffer));
}