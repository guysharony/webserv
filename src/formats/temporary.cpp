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

void	Temporary::socket(size_t socket)
{
	if (this->_sock_id >= 0) {
		Message::error("Socket already defined.");
		return;
	}

	this->_sock_id = static_cast<ssize_t>(socket);
}


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

int		Temporary::isOpen(int id)
{
	for (std::vector<int>::iterator it = this->_opened.begin(); it != this->_opened.end(); it++)
	{
		if (*it == id)
			return (1);
	}

	return (0);
}

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

int			Temporary::clear(int id)
{
	if (this->isOpen(id)) {
		this->_files[id].clear();
		return (1);
	}

	return (0);
}

int			Temporary::close(int id)
{
	std::vector<int>::iterator file;
	if (this->getData(id, file)) {
		this->_opened.erase(file);
		this->_files[id].close();
		return (1);
	}

	return (0);
}

int			Temporary::getData(int id, std::vector<int>::iterator & data)
{
	for (std::vector<int>::iterator it = this->_opened.begin(); it != this->_opened.end(); it++)
	{
		if (*it == id) {
			data = it;
			return (1);
		}
	}

	return (1);
}

int			Temporary::cursor(int id, size_t pos)
{
	if (this->isOpen(id)) {
		this->_files[id].position(pos);
		return (1);
	}

	return (0);
}

int			Temporary::append(int id, std::string value)
{
	if (this->isOpen(id)) {
		this->_files[id].write(value);
		return (1);
	}

	return (0);
}

int			Temporary::read(int fd, std::string & packet)
{
	if (this->isOpen(fd)) {
		this->_files[fd].read(packet);

		return (packet.length() > 0);
	}

	return (0);
}

int			Temporary::copy(std::string dest, int id, bool recreate)
{
	if (!this->isOpen(id))
		return (-1);

	std::string line;
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

int			Temporary::copy(int fd, std::string source)
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

		this->append(fd, toString(buffer));
	}

	::close(fd_source);

	this->cursor(fd, 0);

	if (res < 0) return (0);

	return (1);
}