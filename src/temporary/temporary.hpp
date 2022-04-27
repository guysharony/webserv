#ifndef TEMPORARY_HPP
# define TEMPORARY_HPP

# include "tmpfile.hpp"


class Temporary
{
	public:
		Temporary(Descriptors *descriptors);
		~Temporary();

		bool					create(std::string const & name);
		int					read(std::string const & filename, std::string &dest);
		int					read(std::string const &filename, STRBinary & dest);
		int					append(std::string const &filename, std::string const & source);
		int					append(std::string const &filename, STRBinary source);
		bool					close(std::string const &filename);
		short				getEvents(std::string const &filename);
		ssize_t				getSize(std::string const &filename);
		bool					setEvents(std::string const &filename, short event);
		int					getFD(std::string const &filename);
		int					clear(std::string const &filename);
		void					setDescriptors(Descriptors *descriptors);
		bool					resetCursor(std::string const &filename);

		typedef std::vector<TmpFile*>		tmpfiles_type;
		typedef tmpfiles_type::iterator	tmpfile_type;

	private:
		tmpfiles_type					_tmpfiles;
		Descriptors					*_descriptors;

		Temporary();
		Temporary(Temporary const & src);
		Temporary	&operator=(Temporary const &other);
};

#endif