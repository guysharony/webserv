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
# include "../descriptors/descriptors.hpp"
# include "../formats/files.hpp"
# include "../formats/units.hpp"
# include "../formats/strings.hpp"
# include "../formats/strbinary.hpp"


class TmpFile
{
	public:
		TmpFile(Descriptors *descriptors, std::string const &filename);
		virtual ~TmpFile();

		/* Getters */
		std::string			getFilename(void);
		std::string			getPath(void);
		Descriptors::poll_type	getPoll(void);
		int					getFD(void);
		short				getEvents(void);
		size_t				getSize(void);

		/* Setters */
		void					setEvents(short events);

		void					resetCursor(void);
		int					read(std::string & value);
		int					read(STRBinary & value);
		int					write(std::string const & value);
		int					write(STRBinary value);
		int					clear(void);
		void					close(void);

	private:
		int					_fd;
		size_t				_size;
		size_t				_begin;
		std::string			_path;
		std::string			_filename;

		Descriptors			*_descriptors;

		TmpFile(void);
		TmpFile(TmpFile const & src);
		TmpFile	&operator=(TmpFile const &other);
};

#endif