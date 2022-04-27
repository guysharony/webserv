#ifndef STRINGS_HPP
# define STRINGS_HPP

# include <iostream>
# include <string>
# include <unistd.h>
# include <stdlib.h>
# include <vector>

# include "../../include/constants.hpp"


/* convertions */
std::string				toString(int nbr, bool uppercase = false);
std::string				toString(char c, bool uppercase = false);
std::string				toString(size_t c, bool uppercase = false);
std::string				toString(ssize_t nbr, bool uppercase = false);
std::string				toString(char * str, bool uppercase = false);

char 					*toChar(std::string src);

std::string				toUppercase(std::string value);
std::string 				toLowercase(std::string value);

bool						startWith(std::string str1, std::string str2);
bool						endWith(std::string str1, std::string str2);
ssize_t					contains(std::string str1, std::string str2);
bool						isEmpty(std::string str);


/* parsing */
int						trim(std::string & value);
int						occurence(std::string source, std::string occurence);
int						getFirstLine(std::string source, std::string & line, std::string separator = CRLF, bool withseparator = false);
int						readFileLines(int fd, std::string & line, std::string separator = "\n", bool withseparator = false);
std::vector<std::string>		split(std::string str, std::string delimiter);
void 					replaceAll(std::string  &str, const std::string &from, const std::string& to);


/* formats */
int						isTchar(std::string value);
int						isAlpha(char value);
int						isAlpha(std::string value);
int						isDigit(char value);
int						isAlphaNum(char value);


#endif