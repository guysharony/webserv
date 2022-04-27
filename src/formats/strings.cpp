#include "strings.hpp"


/* convertions */
std::string 	toString(ssize_t nbr, bool uppercase)
{
	std::string number;
	bool 		negative = false;

	if (nbr < 0)
	{
		nbr = -nbr;
		negative = true;
	}

	if (!nbr)
		number.insert(number.begin(), '0');

	while (nbr > 0)
	{
		number.insert(number.begin(), static_cast<char>((nbr % 10) + 48));
		nbr /= 10;
	}

	if (negative == true)
		number.insert(number.begin(), '-');

	return ((uppercase) ? toUppercase(number) : number);
}

std::string 	toString(int nbr, bool uppercase)
{
	return (toString(static_cast<ssize_t>(nbr), uppercase));
}

std::string 	toString(size_t nbr, bool uppercase)
{
	return (toString(static_cast<ssize_t>(nbr), uppercase));
}

std::string 	toString(char c, bool uppercase)
{
	std::string temporary;

	temporary.insert(temporary.begin(), static_cast<char>(c));

	return ((uppercase) ? toUppercase(temporary) : temporary);
}

std::string 	toString(char * str, bool uppercase)
{
	std::string tmp(str);

	return ((uppercase) ? toUppercase(tmp) : tmp);
}

char 		*toChar(std::string src)
{
	char *tmp;

	if (!(tmp = (char *)malloc((sizeof(char) * (src.length() + 1)))))
		return (NULL);

	size_t i = 0;
	for (; i < src.length(); i++)
		tmp[i] = src[i];

	tmp[i] = 0;

	return (tmp);
}

std::string		toUppercase(std::string value)
{
	std::string	tmp;
	size_t		size = value.length();

	for (size_t i = 0; i < size; i++) {
		tmp.push_back((value[i] >= 'a' && value[i] <= 'z') ? (value[i] - 32) : value[i]);
	}

	return (tmp);
}

std::string		toLowercase(std::string value)
{
	std::string	tmp;
	size_t		size = value.length();

	for (size_t i = 0; i < size; i++) {
		tmp.push_back((value[i] >= 'A' && value[i] <= 'Z') ? (value[i] + 32) : value[i]);
	}

	return (tmp);
}

bool			endWith(std::string str1, std::string str2)
{
	size_t	len1 = str1.length();
	size_t	len2 = str2.length();

	if (len1 < len2) return (false);

	return (!str1.substr(len1 - len2, len2).compare(str2));
}

bool			startWith(std::string str1, std::string str2)
{
	size_t	len1 = str1.length();
	size_t	len2 = str2.length();

	if (len1 < len2) return (false);

	return (!str1.substr(0, len2).compare(str2));
}

ssize_t			contains(std::string str1, std::string str2)
{
	size_t	len1 = str1.length();
	size_t	len2 = str2.length();

	if (len1 < len2) return (-1);

	return (str1.find(str2) != std::string::npos ? str1.find(str2) : -1);
}

bool			isEmpty(std::string str)
{
	return (str.length() > 0);
}

/* parsing */
int				trim(std::string & value)
{
	std::string		tmp = value;

	for (size_t i = 0; i < tmp.length(); i++) {
		char tmp_char = value[i];

		if (tmp_char > 32) {
			tmp = tmp.substr(i);
			break;
		}
	}

	for (size_t i = tmp.length(); i != 0; i--) {
		char tmp_char = value[i];

		if (tmp_char > 32) {
			tmp = tmp.substr(0, i + 1);
			break;
		}
	}

	value = tmp;

	return (1);
}

int		occurence(std::string source, std::string occurence)
{
	int			count = 0;
	size_t		pos = 0;

	while ((pos = source.find(occurence, pos)) != std::string::npos) {
		count++;
		pos++;
	}

	return (count);
}

int		getFirstLine(std::string source, std::string & line, std::string separator, bool withseparator)
{
	size_t	pos = source.find(separator);
	int		find = pos != std::string::npos;

	line = ((find) ? source.substr(0, (withseparator) ? pos + separator.length() : pos) : source);

	return (find);
}

int		readFileLines(int fd, std::string & line, std::string separator, bool withseparator)
{
	int					output;
	char					buffer[BUFFER_SIZE + 1];
	std::string			temporary;
	static std::string  	content;

	if (read(fd, buffer, 0) < 0)
		return (-1);

	while ((content.find(separator) == std::string::npos)
	&& (output = read(fd, buffer, BUFFER_SIZE)) > 0)
	{
		buffer[output] = '\0';
		temporary.append(content);
		temporary.append(std::string(buffer));
		content = temporary;
		temporary.clear();
	}

	size_t	sep_pos = content.find(separator);
	size_t	sep_len = separator.length();
	bool	sep_fnd = sep_pos != std::string::npos;

	if (sep_fnd)
	{
		line = content.substr(0, sep_pos + ((withseparator) ? sep_len : 0));
		content = content.substr(content.find(separator) + separator.length());
		return (1);
	}

	line = content;
	content.clear();
	return (0);
}

std::vector<std::string>	split(std::string str, std::string delimiter)
{
	std::vector<std::string> 	tmp;

	size_t	pos = 0;
	while ((pos = str.find(delimiter)) != std::string::npos)
	{
		tmp.push_back(str.substr(0, pos));
		str.erase(0, pos + delimiter.length());
	}

	tmp.push_back(str);

	return (tmp);
}

void 			replaceAll(std::string  &str, const std::string &from, const std::string& to)
{
	if (from.empty())
		return;
		
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::string::npos)
	{
		str.replace(start_pos, from.length(), to);
		start_pos += to.length();
	}
}


/* formats */
int		isTchar(std::string value)
{
	for (size_t i = 0; i < value.length(); i++) {
		char tmp = value[i];

		if (!(tmp == '!' || tmp == '#' || tmp == '$'
		|| tmp == '%' || tmp == '&' || tmp == '\'' || tmp == '*'
		|| tmp == '+' || tmp == '-' || tmp == '.' || tmp == '^'
		|| tmp == '_' || tmp == '`' || tmp == '|' || tmp == '~'
		|| isAlpha(tmp)
		|| isDigit(tmp)))
			return (0);
	}

	return (1);
}

int		isAlpha(char value)
{ return ((value >= 'a' && value <= 'z') || (value >= 'A' && value <= 'Z')); }

int		isDigit(char value)
{ return (value >= '0' && value <= '9'); }

int		isAlphaNum(char value)
{ return (isAlpha(value) && isDigit(value)); }

int		isAlpha(std::string value)
{
	for (size_t i = 0; i < value.length(); ++i) {
		if (!isAlpha(value[i]))
			return 0;
	}

	return 1;
}