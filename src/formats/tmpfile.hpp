#ifndef TMPFILE_HPP
# define TMPFILE_HPP

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


class TmpFile
{
	public:
		TmpFile(std::string const &filename);
		~TmpFile();

		/* Getters */
		std::string		getFilename(void);
		std::string		getPath(void);

		void				resetCursor(void);
		int				read(std::string & value);
		int				write(std::string const & value);
		int				display(void);

	private:
		int				_fd;
		std::string		_path;
		std::string		_filename;

		std::string		_generate_filepath(void);

		TmpFile(void);
		TmpFile(TmpFile const & src);
		TmpFile	&operator=(TmpFile const &other);
};

#endif