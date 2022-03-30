#include "temporary.hpp"


Temporary::Temporary(void)
:
	_sock_id(-1),
	_file_id(0)
{ }

Temporary::Temporary(ssize_t socket)
:
	_sock_id(socket),
	_file_id(0)
{ }

Temporary::Temporary(Temporary const & src)
{ *this = src; }

Temporary::~Temporary()
{
	for (file_type it = this->_files.begin(); it != this->_files.end(); it++)
		it->close();

	this->_files.clear();
}

/**
 * @brief Defining client socket (fle descriptor) of current class.
 * 
 * @param socket client socket
 */
void	Temporary::socket(size_t socket)
{
	if (this->_sock_id >= 0) {
		Message::error("Socket already defined.");
		return;
	}

	this->_sock_id = static_cast<ssize_t>(socket);
}

/**
 * @brief Get the size of a temporary file ID.
 * 
 * @param id The id of temporary file.
 * @return size_t Size of temporary file.
 */
size_t	Temporary::size(int id)
{
	if (id < 0)
		return (this->_files.size());

	if (!this->isOpen(id))
		return (-1);

	return (this->_files[id].size());
}

bool			Temporary::_isSocketDefined(void) {
	if (this->_sock_id < 0) {
		Message::error("Socket is not defined.");
		return false;
	}

	return true;
}

int			Temporary::_createFilename(std::string & filename)
{
	if (this->_isSocketDefined()) {
		filename = "/tmp/serv_" + intToHex(this->_sock_id) + "_" + intToHex(this->_file_id);
		this->_file_id++;

		return 1;
	}

	return 0;
}

int			Temporary::_filename(std::string & filename)
{
	std::string tmp_name;

	if (!this->_createFilename(tmp_name))
		return 0;

	while (exists(tmp_name)) {
		this->_createFilename(tmp_name);
	}

	filename = tmp_name;

	return (1);
}

/**
 * @brief Check if a temporary file with a specific id is open.
 * 
 * @param id ID of temporary file.
 * @return int 1 if file if open, 0 otherwise.
 */
int		Temporary::isOpen(int id)
{
	for (std::vector<int>::iterator it = this->_opened.begin(); it != this->_opened.end(); it++)
	{
		if (*it == id)
			return (1);
	}

	return (0);
}

/**
 * @brief Creating a temporary file with a specific ID.
 * 
 * @param id The id of the temporary file.
 * @param data Result of created file.
 * @return int 1 if temporary file was created successfully, 0 otherwise.
 */
int		Temporary::create(int id, TmpFile & data)
{
	if (!this->isOpen(id)) {
		std::string name;

		if (!this->_filename(name)) return (0);

		data.create(name);

		this->_opened.push_back(id);
		this->_files.resize(id);
		this->_files.insert(this->_files.begin() + id, data);
	}

	return (1);
}

/**
 * @brief Creating a temporary file with a specific ID.
 * 
 * @param id The id of the temporary file.
 * @return int 1 if temporary file was created successfully, 0 otherwise.
 */
int		Temporary::create(int id)
{
	if (!this->isOpen(id)) {
		std::string name;

		if (!this->_filename(name)) return (0);

		TmpFile data;
		data.create(name);

		this->_opened.push_back(id);
		this->_files.resize(id);
		this->_files.insert(this->_files.begin() + id, data);
	}
	
	return (1);
}

/**
 * @brief Clear the content of temporary file without closing it.
 * 
 * @param id The ID of the temporary file to clear.
 * @return int 1 if temporary file was cleared successfully, 0 otherwise.
 */
int			Temporary::clear(int id)
{
	if (this->isOpen(id)) {
		this->_files[id].clear();
		return (1);
	}

	return (0);
}

/**
 * @brief Close and delete a specific temporary file. (note: All temporary files are automatically deleted are deconstructor)
 * 
 * @param id ID of temporary file to delete.
 * @return int 1 if temporary file was closed successfully, 0 otherwise.
 */
int			Temporary::close(int id)
{
	if (this->isOpen(id)) {
		this->_files[id].clear();
		this->_files[id].close();
		return (1);
	}

	return (0);
}

/**
 * @brief Change the cursor position (working only for reading file).
 * 
 * @param id ID of temporary file.
 * @param pos New cursor position.
 * @return int 1 if success, 0 otherwise.
 */
int			Temporary::cursor(int id, size_t pos)
{
	if (this->isOpen(id)) {
		this->_files[id].position(pos);
		return (1);
	}

	return (0);
}

/**
 * @brief Append string to temporary file.
 * 
 * @param id ID of temporary file.
 * @param value String to append.
 * @return int 1 if success, 0 otherwise.
 */
int			Temporary::append(int id, std::string value)
{
	if (this->isOpen(id)) {
		this->_files[id].write(value);
		return (1);
	}

	return (0);
}

/**
 * @brief Read content of temporary file (max 1000 characters so use it with a while()).
 * 
 * @param id ID of temporary file.
 * @param packet Result of read.
 * @return int 0 of the end of file wa reached, 1 otherwise.
 */
int			Temporary::read(int id, std::string & packet)
{
	if (this->isOpen(id)) {
		this->_files[id].read(packet);

		return (packet.length() > 0);
	}

	return (0);
}

/**
 * @brief Copy content of temporary file into another file.
 * 
 * @param dest Path of destination file.
 * @param id ID of temporary file to copy.
 * @param recreate true to remove the content of destination file before copying, false otherwise.
 * @return int -1 if ID doesn't exist, 0 on error and 1 on success.
 */
int			Temporary::copy(std::string dest, int id, bool recreate)
{
	if (!this->isOpen(id))
		return (-1);

	std::string	line;
	int			fd_dest;

	if ((fd_dest = open(dest.c_str(), O_WRONLY | O_CREAT | ((recreate) ? O_TRUNC : O_APPEND), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)) < 1)
		return (0);

	this->cursor(id, 0);

	while (this->_files[id].read(line))
		write(fd_dest, line.c_str(), line.length());

	write(fd_dest, line.c_str(), line.length());

	::close(fd_dest);

	return (1);
}

/**
 * @brief Copy content of source file into temporary file.
 * 
 * @param id ID of temporary file.
 * @param source Path to source file.
 * @return int 1 on success, 0 otherwise.
 */
int			Temporary::copy(int id, std::string source)
{
	int			res;
	std::string 	line;
	char			buffer[2048];
	int			fd_source;

	if ((fd_source = open(source.c_str(), O_RDONLY)) < 1)
		return (0);

	lseek(fd_source, 0, SEEK_SET);

	while ((res = ::read(fd_source, buffer, 2047)) > 0) {
		buffer[res] = 0;

		this->append(id, toString(buffer));
	}

	::close(fd_source);

	this->cursor(id, 0);

	if (res < 0) return (0);

	return (1);
}

/**
 * @brief Display content of temporary file for debugging.
 * 
 * @param id ID of temporary file.
 * @return int 1 on success, 0 otherwise.
 */
int			Temporary::display(int id)
{
	std::string line;

	if (this->isOpen(id)) {
		this->_files[id].display();

		return (1);
	}

	return (0);
}