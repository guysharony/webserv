#ifndef FILES_HPP
# define FILES_HPP

# include <iostream>
# include <string>
# include <unistd.h>
# include <vector>
# include <dirent.h>
# include <sys/stat.h>
# include <cstdlib>
# include <ctime>
# include <cstdio>
# include <fcntl.h>
# include <sys/time.h>
# include <fstream>
# include "units.hpp"
# include "strings.hpp"

int						getAbsolutePath(std::string path, std::string & absolute);
std::string				secureAddress(std::string path, std::string filename);
std::vector<std::string> 	listFromDirectory(std::string src);

int						exists(std::string path);
int						isFile(std::string path);
int						isDirectory(std::string path);

std::string				randomString(size_t length = 20);
std::string				uniqueFilename(std::string path, size_t length = 20);
int						uniqueFile(std::string path, int flags, mode_t mode);
int						uniqueFile(std::string path, std::string & filename, int flags);
int						uniqueFile(std::string path, std::string & filename, int flags, mode_t mode);

#endif