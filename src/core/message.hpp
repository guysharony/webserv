#ifndef MESSAGE_HPP
# define MESSAGE_HPP

# include <iostream>
# include <string>
# include "../formats/strings.hpp"

class Message
{
	public:
		Message(void);
		Message(std::string const & message);
		Message(Message const & message);
		~Message();

		static void	error(std::string const & message);
		
		template <class T>
		static void	debug(T const & message)
		{
			#ifdef DEBUG
			std::cout << message;
			#else
			(void)message;
			#endif
		}

		template <class T>
		static void	debugln(T const & message)
		{
			debug(message);
			debug("\n");
		}

		static void	bindError(std::string host, std::string port, int error);

	private:
	
		class ErrorException : public std::exception
		{
			public:
				ErrorException(Message const & message) : _error(message._message) { };
				virtual ~ErrorException() throw() {};
				virtual const char* what() const throw() { return (_error.c_str()); };

			private:
				std::string _error;
		};

		std::string _message;
};

#endif
