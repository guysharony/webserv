#ifndef UNITS_HPP
# define UNITS_HPP

# include <iostream>
# include <string>
# include <vector>

/* integers */
int 				toInteger(std::string str);
int 				toInteger(char c);
int 				toInteger(char * str);

/* floats */
float 			toFloat(std::string str);

/* units */
ssize_t			fromSizeUnit(std::string num);
int 				hexToInt(std::string num);;
std::string		intToHex(int num);

#endif