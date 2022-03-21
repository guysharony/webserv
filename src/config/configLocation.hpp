#ifndef CONFIG_LOCATION_HPP
# define CONFIG_LOCATION_HPP

# include <iostream>
# include <string>
# include <unistd.h>
# include <fcntl.h>
# include <list>
# include <vector>
# include <map>
# include "../core/message.hpp"

class ConfigLocation
{
	public:
		ConfigLocation(void);
		ConfigLocation(ConfigLocation const & src);
		~ConfigLocation();

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

		struct 								configuration_struct
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

		typedef std::list<configuration_struct>		configurations_type;
		typedef configurations_type::iterator		configuration_type;

		configurations_type						configuration;
		std::string							compressed_configuration_file;
		std::string::iterator					compressed_configuration_file_iterator;

	protected:
		void									parseLocation(configuration_struct &config);

	private:
		// Directives
		void									_parseMethods(location_struct &location);
		void									_parseRoot(location_struct &location);
		void									_parseIndex(location_struct &location);
		void									_parseAutoIndex(location_struct &location);
		void									_parseClientMaxBodySize(location_struct &location);
		void									_parseRedirect(location_struct &location);
		void									_parseErrorPage(location_struct &location);
		void									_parseCGIPath(location_struct &location);
		void									_parseCGIExtentions(location_struct &location);

		std::string 							_extractWord(void);
};

#endif