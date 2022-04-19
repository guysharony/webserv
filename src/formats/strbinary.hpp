#ifndef STR_BINARY_HPP
# define STR_BINARY_HPP

# include <iostream>
# include <string>
# include <vector>

class STRBinary {

	public:
		STRBinary(void);
		STRBinary(std::string str);
		STRBinary(const char * str);
		STRBinary(STRBinary const & other);
		~STRBinary();

		size_t		find(std::string str);

		void			append(const char c);
		void			append(std::string str);
		void			append(std::vector<char> & other);
		void			append(const char * str, size_t size);

		void			clear(void);

		STRBinary		substr(size_t from, size_t to);
		STRBinary		substr(size_t from);

		std::string	str(void);
		size_t		length(void);

	private:
		std::vector<char>	_data;

};

#endif