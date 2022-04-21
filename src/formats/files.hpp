#ifndef FILES_HPP
# define FILES_HPP

# include <iostream>
# include <string>
# include <unistd.h>
# include <vector>
# include <dirent.h>
# include <sys/stat.h>
# include "strings.hpp"

int						getAbsolutePath(std::string path, std::string & absolute);
std::string				secureAddress(std::string path, std::string filename);
std::vector<std::string> 	listFromDirectory(std::string src);

int						exists(std::string path);
int						isFile(std::string path);
int						isDirectory(std::string path);

#endif