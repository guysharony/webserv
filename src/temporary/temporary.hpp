#ifndef TEMPORARY_HPP
# define TEMPORARY_HPP

# include "tmpfile.hpp"


class Temporary
{
	public:
		Temporary();
		Temporary(Descriptors *descriptors);
		~Temporary();

		bool					create(std::string const & name);
		int					read(std::string const & filename, std::string &dest);
		int					append(std::string const &filename, std::string const & source);
		int					display(std::string const &filename);
		void					close(std::string const &filename);
		short				getEvents(std::string const &filename);
		void					setEvents(std::string const &filename, short event);
		void					setDescriptors(Descriptors *descriptors);
		void					resetCursor(std::string const &filename);

		typedef std::vector<TmpFile*>		tmpfiles_type;
		typedef tmpfiles_type::iterator	tmpfile_type;

	private:
		tmpfiles_type					_tmpfiles;
		Descriptors					*_descriptors;

		Temporary(Temporary const & src);
		Temporary	&operator=(Temporary const &other);
};

#endif