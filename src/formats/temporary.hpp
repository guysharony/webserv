#ifndef TEMPORARY_HPP
# define TEMPORARY_HPP

# include "../core/message.hpp"
# include "files.hpp"
# include "tmpfile.hpp"


class Temporary
{
	public:
		Temporary(void);
		Temporary(ssize_t socket);
		Temporary(Temporary const & src);
		~Temporary();

		typedef std::vector<TmpFile>	files_type;
		typedef files_type::iterator	file_type;

		size_t				size(int id = -1);
		int					create(int id);
		int					clear(int id);
		int					create(int id, TmpFile & data);
		int					read(int id, std::string & packet);
		int					cursor(int id, size_t pos);
		int					append(int id, std::string value);

		int					close(int id);
		int					isOpen(int id);

		int					copy(std::string dest, int id, bool recreate = false);
		int					copy(int id, std::string dest);

		void					socket(size_t socket);
		int					display(int id);

	private:
		int					_createFilename(std::string & filename);
		bool					_isSocketDefined(void);
		int					_filename(std::string & filename);
		ssize_t				_sock_id;
		int					_file_id;
		std::vector<int>		_opened;
		files_type			_files;
};

#endif