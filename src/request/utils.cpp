#include "request.hpp"

std::string trim2(const std::string& str)
{
    const char* t = " \t\n\r\f\v";
    size_t first = str.find_first_not_of(t);
    if (std::string::npos == first)
        return str;
    size_t last = str.size() - 1;
    return str.substr(first, (last - first + 1));
}

int	ft_isspace(char c)
{
	if (c == ' ' || c == '\t' || c == '\f' ||
	c == '\v' || c == '\n' || c == '\r')
		return (1);
	return (0);
}

int	ft_atoi(const char *nptr)
{
	int				sign;
	long long int	result;
	int				index;

	sign = 1;
	result = 0;
	index = 0;
	while (ft_isspace(nptr[index]) == 1)
		index++;
	if (nptr[index] == '-' || nptr[index] == '+')
	{
		if (nptr[index] == '-')
			sign = sign * -1;
		index++;
	}
	while (nptr[index] >= '0' && nptr[index] <= '9')
	{
		result = result * 10 + (nptr[index] - '0');
		index++;
		if (result > 2147483648 && sign == 1)
			return (-1);
		else if (result > 2147483648 && sign == -1)
			return (-1);
	}
	return (sign * result);
}

int	ft_isalpha(const char * str)
{
	while(*str != '\0')
	{
		if ((*str >= 'a' && *str <= 'z') || (*str >= 'A' && *str <= 'Z'))
			return (1);
		else
			str++;
	}
	return 0;
}

void print_buffer(std::string buffer, size_t max_size, std::string color){
	if (buffer.size() > max_size + 50){
		std::cout<<color<<buffer.substr(0, max_size)<< "..."<<std::endl;
		std::cout<<color<<buffer.substr(buffer.size() - 50, buffer.size())<<RESET<<std::endl;
	}
	else{
		std::cout<<color<<buffer<<RESET<<std::endl;
	}
	
}

int		isFiley(std::string path)
{
	struct stat status;
	if (stat(path.c_str(), &status) == 0 )
	{
		if (status.st_mode & S_IFDIR)
			return 2;
		else if (status.st_mode & S_IFREG)
			return 1;
	}
	return 0;
}

std::string		readHtmlFile(std::string path)
{
	std::ofstream		file;
	std::stringstream	buffer;

	if (isFiley(path))
	{
		file.open(path.c_str(), std::ifstream::in);
		if (file.is_open() == false)
			return ("<!DOCTYPE html>\n<html><title>404</title><body>error page not found !!</body></html>\n");
		buffer << file.rdbuf();
		file.close();
		return (buffer.str());
	}
	else
			return ("<!DOCTYPE html>\n<html><title>404</title><body>error page not found !!</body></html>\n");
		
}

std::string	intToStr(int a){
	std::stringstream ss;
    ss << a;
    std::string str = ss.str();
	return (str);
}

int existingDir(std::string path){
    DIR *dir = opendir(path.c_str());
    if (dir){
		closedir(dir);
        return 1;
	}
	closedir(dir);
    return 0;
}