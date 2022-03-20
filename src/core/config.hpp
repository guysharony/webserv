#ifndef CONFIG_HPP
# define CONFIG_HPP

# include <iostream>
# include <string>
# include <unistd.h>
# include <fcntl.h>
# include <list>
# include <vector>
# include <map>
# include "message.hpp"

class Config
{
	public:
		Config(void);
		Config(Config const & src);
		~Config();

		int		load(char *filename);
		int		load(std::string filename);

		struct 								location_struct
		{
			std::string 						root;
			std::string 						location;
			std::string						redirect;
			std::string						cgi_path;
			std::list<std::string>				cgi_extentions;

			std::list<std::string> 				index;
			int								auto_index;
			std::map<std::string, std::string> 	error_page;
			std::list<std::string> 				methods;

			std::string 						client_max_body_size;
		};

		typedef std::list<location_struct>			locations_type;
		typedef locations_type::iterator			location_type;

		struct 								config_struct
		{
			std::string 						server_name;
			std::string 						port;
			std::string 						host;
			std::string 						root;
			std::string 						client_max_body_size;
			std::string						redirect;
			std::string						cgi_path;
			std::list<std::string>				cgi_extentions;

			std::list<std::string> 				index;
			int								auto_index;
			std::map<std::string, std::string> 	error_page;

			locations_type 					locations;
		};

		typedef std::list<config_struct>			configs_type;
		typedef configs_type::iterator			config_type;

		configs_type							conf;

	private:
		std::string		_filename;

		int				_parseConfiguration(void);
		void				_parseConfigurationLine(int &brakets, std::string const line, std::string &result);
		void				_addSpaceAfterLimiter(int i, std::string &result);

		bool				_isLimiter(int &brackets, char character);
		bool				_isSpace(char character);
		bool				_isComment(char character);
};

#endif