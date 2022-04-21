#include "files.hpp"


int						getAbsolutePath(std::string path, std::string & absolute)
{
	char		buffer[2048];

	char 	*resolved = realpath(path.c_str(), buffer);

	if (resolved == NULL)
		return (0);

	absolute = toString(resolved);
	
	return (1);
}

std::string				secureAddress(std::string path, std::string filename)
{
	if (path[path.length() - 1] == '/' && filename[0] == '/')
		filename = filename.substr(1);
	else if (path[path.length() - 1] != '/' && filename[0] != '/')
		filename = "/" + filename;
	return path + filename;
}

std::vector<std::string> 	listFromDirectory(std::string src)
{
	std::vector<std::string>	list;

	DIR 						*dir;
	struct dirent 				*ent;
	if ((dir = opendir(src.c_str())) != NULL) {
		while ((ent = readdir(dir)) != NULL)
			list.push_back(toString(ent->d_name));
		closedir(dir);
	}

	return list;
}

int							exists(std::string path)
{
	struct stat s;
	return (stat(path.c_str(), &s) != -1);
}

int							isFile(std::string path)
{
	struct stat s;
	return (stat(path.c_str(), &s) != -1 && s.st_mode & S_IFREG);
}

int							isDirectory(std::string path)
{
	struct stat s;
	return (stat(path.c_str(), &s) != -1 && s.st_mode & S_IFDIR);
}