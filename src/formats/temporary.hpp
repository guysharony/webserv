#ifndef TEMPORARY_HPP
# define TEMPORARY_HPP

# include <iostream>
# include <string>
# include <cstdlib>
# include <ctime>
# include <cstdio>
# include <stdlib.h>
# include <fcntl.h>
# include <sys/stat.h>
# include <unistd.h>
# include <vector>
# include <sys/time.h>
# include <sys/socket.h>
# include <fstream>
# include "../core/message.hpp"
# include "files.hpp"
# include "units.hpp"
# include "strings.hpp"
# include "tmpfile.hpp"


class Temporary
{
	public:
		Temporary(void);
		~Temporary();

		bool					create(std::string const & name);
		int					read(std::string const & filename, std::string &dest);
		int					append(std::string const &filename, std::string const & source);
		int					display(std::string const &filename);
		void					close(std::string const &filename);
		void					resetCursor(std::string const &filename);

		typedef std::vector<TmpFile*>		tmpfiles_type;
		typedef tmpfiles_type::iterator	tmpfile_type;

	private:
		tmpfiles_type		_tmpfiles;

		Temporary(Temporary const & src);
		Temporary	&operator=(Temporary const &other);
};

#endif