#ifndef TMPFILE_HPP
# define TMPFILE_HPP

# include <iostream>
# include <string>
# include <cstdio>
# include <stdlib.h>
# include <fcntl.h>
# include <sys/stat.h>
# include <unistd.h>
# include <vector>
# include <sys/time.h>
# include <sys/socket.h>
# include <fstream>
# include "files.hpp"
# include "units.hpp"
# include "strings.hpp"


class TmpFile
{
	public:
		TmpFile(void);
		TmpFile(TmpFile const & src);
		~TmpFile();

		TmpFile			&operator=(const TmpFile &src);

		int				create(std::string filename);
		size_t			size(void);
		int				clear(void);
		int				close(void);
		int				read(std::string & value);
		int				write(std::string value);
		void				position(ssize_t value);
		size_t			getPosition(void);
		std::string		getFilename(void);

	private:
		size_t			_position;
		std::string		_filename;
};

#endif