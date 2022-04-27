#ifndef UNITS_HPP
# define UNITS_HPP

# include <iostream>
# include <string>
# include <vector>
# include <cstdlib>
# include <sys/types.h>

/* integers */
int				toInteger(std::string str);
int				toInteger(char c);
int				toInteger(char * str);
bool				isPositiveBase10(std::string num);
bool				isPositiveBase16(std::string num);

/* floats */
float 			toFloat(std::string str);

/* units */
ssize_t			fromSizeUnit(std::string num);
int				hexToInt(std::string num);;
std::string		intToHex(int num);
std::string		toBase62(int num);

#endif