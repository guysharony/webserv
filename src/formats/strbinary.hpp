#ifndef STR_BINARY_HPP
# define STR_BINARY_HPP

# include <iostream>
# include <string>
# include <vector>
# include <cstdlib>

class STRBinary {

	public:
		STRBinary(void);
		STRBinary(std::string str);
		STRBinary(const char * str);
		STRBinary(STRBinary const & other);
		~STRBinary();

		STRBinary		&operator=(STRBinary const &rhs);
		STRBinary		&operator=(std::vector<char> const &rhs);
		STRBinary		&operator=(std::string & rhs);
		char			operator[](size_t n);

		size_t		find(std::string str);

		void			append(const char c);
		void			append(std::string str);
		void			append(STRBinary other);
		void			append(std::vector<char> & other);
		void			append(const char * str, size_t size);

		void			clear(void);

		STRBinary		substr(size_t from, size_t to);
		STRBinary		substr(size_t from);

		std::string	str(void);
		char			*c_str(void);

		size_t		length(void);

	private:
		std::vector<char>	_data;

};

std::basic_ostream<char>	&operator<<(std::basic_ostream<char> &value1, STRBinary &value2);

#endif