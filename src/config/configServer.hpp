#ifndef CONFIG_SERVER_HPP
# define CONFIG_SERVER_HPP

# include "configLocation.hpp"

class ConfigServer: public ConfigLocation
{
	public:
		ConfigServer(void);
		ConfigServer(ConfigServer const & src);
		virtual ~ConfigServer();

	protected:
		void				parseServer(void);

	private:
		void				_parseListen(configuration_struct &config);
		void				_parseServerNames(configuration_struct &config);
		void				_parseRoot(configuration_struct &config);
		void				_parseIndex(configuration_struct &config);
		void				_parseAutoIndex(configuration_struct &config);
		void				_parseClientMaxBodySize(configuration_struct &config);
		void				_parseRedirect(configuration_struct &config);
		void				_parseErrorPage(configuration_struct &config);
		void				_parseCGIPath(configuration_struct &config);
		void				_parseCGIExtentions(configuration_struct &config);

		void				_adjustConfiguration(configuration_struct &config);
		void				_adjustRoot(configuration_struct &config);
		void				_adjustIndex(configuration_struct &config);
		void				_adjustErrorPages(configuration_struct &config);
		void				_adjustLocations(configuration_struct &config);

		std::string 		_extractWord(void);
};

#endif