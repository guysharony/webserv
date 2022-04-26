#include "units.hpp"

int 			toInteger(std::string str)
{
	int 		a;
	int 		b;
	int 		c;

	a = 0;
	b = 0;
	c = 0;
	while (str[a] == ' ' || (str[a] >= 9 && str[a] <= 13))
		a++;
	if (str[a] == '-')
	{
		b = 1;
		a++;
	}
	else if (str[a] == '+')
		a++;
	while (str[a] >= 48 && str[a] <= 57)
	{
		c *= 10;
		c += str[a] - 48;
		a++;
	}
	if (b == 1)
		return (-c);
	return (c);
}

int 			toInteger(char c)
{
	return (static_cast<int>(c));
}

int 			toInteger(char * str)
{
	return (toInteger(std::string(str)));
}

int 			toSize(char * str)
{
	return (toInteger(std::string(str)));
}

float 			toFloat(std::string str)
{
	size_t		pos;
	float		result;

	if ((pos = str.find(".")) != std::string::npos)
	{
		float a = static_cast<float>(toInteger(str.substr(0, pos)));
		float b = static_cast<float>(toInteger(str.substr(pos + 1)));

		if (a < 0)
			result = a - (b / 10);
		else
			result = a + (b / 10);
	}
	else
	{
		result = static_cast<float>(toInteger(str));
	}

	return (result);
}

bool				isPositiveBase10(std::string num) {
	for (size_t i = 0; i < num.length(); i++) {
		if (num[i] < '0' || num[i] > '9')
			return false;
	}

	return true;
}

bool				isPositiveBase16(std::string num) {
	for (size_t i = 0; i < num.length(); i++) {
		if (!((num[i] >= '0' && num[i] <= '9') || (num[i] >= 'a' && num[i] <= 'f') || (num[i] >= 'A' && num[i] <= 'F')))
			return false;
	}

	return true;
}

ssize_t			fromSizeUnit(std::string num)
{
	ssize_t 	size = 0;
	int 		pos = 0;
	char 		c;

	while ((c = num[pos]) && (c >= '0' && c <= '9'))
	{
		size = size * 10 + (static_cast<ssize_t>(c) - 48);
		pos++;
	}

	std::string unit = num.substr(pos);

	if (!unit.compare("") || !unit.compare("b") || !unit.compare("Byte") || !unit.compare("byte"))
		return (size);

	if (!unit.compare("K") || !unit.compare("k") || !unit.compare("KB") || !unit.compare("kb") || !unit.compare("Kilobyte") || !unit.compare("kilobyte"))
		return (size * 1000);

	if (!unit.compare("M") || !unit.compare("m") || !unit.compare("MB") || !unit.compare("mb") || !unit.compare("Megabyte") || !unit.compare("megabyte"))
		return (size * 1000000);

	if (!unit.compare("G") || !unit.compare("g") || !unit.compare("GB") || !unit.compare("gb") || !unit.compare("Gigabyte") || !unit.compare("gigabyte"))
		return (size * 1000000000);

	return (-1);
}

int 			hexToInt(std::string num)
{
	size_t	len = num.length();
	int 	base = 1;
	int 	temp = 0;
	for (size_t i = len; i > 0; i--)
	{
		if (num[i - 1] > 47 && num[i - 1] < 58)
			temp += (num[i - 1] - 48) * base;
		else if (num[i - 1] > 64 && num[i - 1] < 71)
			temp += (num[i - 1] - 55) * base;
		else if (num[i - 1] > 96 && num[i - 1] < 103)
			temp += (num[i - 1] - 87) * base;
		base *= 16;
	}

	return (temp);
}

std::string		intToHex(int num)
{
	std::string buffer;
	int			tmp;
	bool		neg = num < 0;

	if (neg)
		num = -num;

	if (!num)
		return ("0");

	while (num > 0) {
		tmp = num % 16;

		if (tmp < 10)
			buffer.insert(buffer.begin(), tmp + 48);
		else
			buffer.insert(buffer.begin(), tmp + 55);

		num /= 16;
	}

	if (neg)
		buffer.insert(buffer.begin(), '-');

	return (buffer);
}

std::string		toBase62(int num)
{
	std::string	buffer;
	int			tmp;
	bool			neg = num < 0;

	if (neg)
		num = -num;

	if (!num)
		return ("0");

	while (num > 0) {
		tmp = num % 62;

		if (tmp < 10)
			buffer.insert(buffer.begin(), tmp + 48);
		else if (tmp >= 10 && tmp < 36)
			buffer.insert(buffer.begin(), tmp + 55);
		else {
			buffer.insert(buffer.begin(), tmp + 61);
		}

		num /= 62;
	}

	if (neg)
		buffer.insert(buffer.begin(), '-');

	return (buffer);
}