#ifndef CONFIG_LOCATION_HPP
# define CONFIG_LOCATION_HPP

# include <iostream>
# include <string>
# include <unistd.h>
# include <fcntl.h>
# include <list>
# include <vector>
# include <map>
# include <set>
# include <exception>
# include "../core/message.hpp"
# include "../formats/strings.hpp"
# include "../formats/units.hpp"
# include "../formats/http.hpp"
# include "../formats/files.hpp"

class ConfigLocation
{
	public:
		class ServerNotFoundException : public std::exception
		{
			public:
			const char * what () const throw ()
			{
				return "Server Not Found";
			}
		};

		class LocationNotFoundException : public std::exception
		{
			public:
			const char * what () const throw ()
			{
				return "Location Not Found";
			}
		};

		class MethodNotAllowed : public std::exception
		{
			public:
			const char * what () const throw ()
			{
				return "Method not allowed";
			}
		};

		ConfigLocation(void);
		ConfigLocation(ConfigLocation const & src);
		virtual ~ConfigLocation();

		typedef std::map<int, std::string>					error_pages_type;
		typedef std::set<std::string>						ports_type;
		typedef std::map<std::string, std::set<std::string> >	listen_type;
		typedef std::vector<int>							methods_type;

		struct 										location_struct
		{
			std::string 								root;
			std::string 								location;
			std::string								redirect;
			std::string								cgi_path;
			std::vector<std::string>						cgi_extentions;

			std::vector<std::string> 					index;
			int										auto_index;
			error_pages_type 							error_page;
			methods_type 								methods;

			ssize_t 									client_max_body_size;
		};

		typedef std::list<location_struct>					locations_type;
		typedef locations_type::iterator					location_type;

		struct 										configuration_struct
		{
			std::vector<std::string> 					server_names;
			listen_type								listen;
			std::string 								root;
			ssize_t 									client_max_body_size;
			std::string								redirect;
			std::string								cgi_path;
			std::vector<std::string>						cgi_extentions;

			std::vector<std::string> 					index;
			int										auto_index;
			error_pages_type 							error_page;

			locations_type 							locations;
		};

		typedef std::list<configuration_struct>				configurations_type;
		typedef configurations_type::iterator				configuration_type;

		configurations_type								configuration;
		std::string									compressed_configuration_file;
		std::string::iterator							compressed_configuration_file_iterator;

	protected:
		void											parseLocation(configuration_struct &config);

	private:
		void											_parseMethods(location_struct &location);
		void											_parseRoot(location_struct &location);
		void											_parseIndex(location_struct &location);
		void											_parseAutoIndex(location_struct &location);
		void											_parseClientMaxBodySize(location_struct &location);
		void											_parseRedirect(location_struct &location);
		void											_parseErrorPage(location_struct &location);
		void											_parseCGIPath(location_struct &location);
		void											_parseCGIExtentions(location_struct &location);

		std::string 									_extractWord(void);
};

#endif
