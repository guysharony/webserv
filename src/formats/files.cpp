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

std::string					randomString(size_t length)
{
	std::string	filename;

	for (size_t i = 0; i < length; ++i)
		filename.append(toBase62(rand() % 62 + 1));

	return filename;
}

std::string					uniqueFilename(std::string path, size_t length) {
	std::string	name;

	name = secureAddress(path, randomString(length));

	return exists(name) ? uniqueFilename(path, length) : name;
}

int							uniqueFile(std::string path, std::string & filename, int flags)
{
	std::string	tmp;

	if (!exists(path))
		mkdir(path.c_str(), 0700);
	else if (isFile(path))
		return -1;

	tmp = uniqueFilename(path);
	filename = tmp.substr(path.length());

	return open(tmp.c_str(), flags);
}

int							uniqueFile(std::string path, int flags, mode_t mode)
{
	if (!exists(path))
		mkdir(path.c_str(), 0700);
	else if (isFile(path))
		return -1;

	return open(uniqueFilename(path).c_str(), flags, mode);
}

int							uniqueFile(std::string path, std::string & filename, int flags, mode_t mode)
{
	std::string	tmp;

	if (!exists(path))
		mkdir(path.c_str(), 0700);
	else if (isFile(path))
		return -1;

	tmp = uniqueFilename(path);
	filename = tmp.substr(path.length());

	return open(tmp.c_str(), flags, mode);
}