#ifndef TEMPORARY_HPP
# define TEMPORARY_HPP

# include "files.hpp"
# include "tmpfile.hpp"


class Temporary
{
	public:
		Temporary(void);
		Temporary(Temporary const & src);
		~Temporary();

		typedef std::vector<TmpFile>	files_type;
		typedef files_type::iterator	file_type;

		size_t				size(int fd = -1);
		int					create(int fd);
		int					clear(int fd);
		int					create(int fd, TmpFile & data);
		int					getData(int fd, std::vector<int>::const_iterator data);
		int					isFD(int id);
		int					read(int fd, std::string & packet);
		int					cursor(int fd, size_t pos);
		int					append(int fd, std::string value);

		int					close(int id);
		int					isOpen(int id);

		int					copy(std::string dest, int fd, bool recreate = false);
		int					copy(int fd, std::string dest);

		void					socket(int socket);

	private:
		std::string			_createFilename(void);
		int					_filename(std::string & filename);
		int					_sock_id;
		int					_file_id;
		std::vector<int>		_opened;
		files_type			_files;
};

#endif