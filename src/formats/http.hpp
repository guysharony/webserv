#ifndef HTTP_HPP
# define HTTP_HPP

# include <iostream>
# include <string>
# include <sys/time.h>
# include "../../include/constants.hpp"

/* status */
int 				isHttpStatus(int status);
std::string		getHttpStatusMessage(int status);

/* methods */
int				isHttpMethod(std::string value);
int				isHttpMethod(std::string value, int & method);
std::string 		getDate(std::string format = "%a, %d %b %G %T GMT");

#endif