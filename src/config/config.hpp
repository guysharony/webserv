#ifndef CONFIG_HPP
# define CONFIG_HPP

# include <iostream>
# include <string>
# include <unistd.h>
# include <fcntl.h>
# include <list>
# include <vector>
# include <map>
# include "configServer.hpp"

class Config: public ConfigServer
{
	public:
		Config(void);
		Config(Config const & src);
		~Config();

		int		load(char *filename);
		int		load(std::string filename);

	private:
		std::string		_filename;

		int				_parseConfiguration(void);
		void				_parseConfigurationLine(int &brakets, std::string const line);

		void				_addSpaceAfterLimiter(int i);

		bool				_isLimiter(int &brackets, char character);
		bool				_isSpace(char character);
		bool				_isComment(char character);
};

#endif