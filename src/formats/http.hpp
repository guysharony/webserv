#ifndef HTTP_HPP
# define HTTP_HPP

# include <iostream>
# include <string>
# include "../../include/constants.hpp"

/* status */
int 				isHttpStatus(int status);
std::string		getHttpStatusMessage(int status);

/* methods */
int				isHttpMethod(std::string value, int & method);

#endif