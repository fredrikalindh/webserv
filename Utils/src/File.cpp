#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/param.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <unistd.h>

#include <Utils/File.hpp>
#include <iostream>

namespace
{
static const size_t MAX_BLOCK_COPY_SIZE = 65536;
}
namespace Utils
{
File::File(std::string path) : path_(path),
							   handle_(-1),
							   writeAccess(false) {}
File::File(const File &x) : path_(x.path_),
							handle_(-1),
							writeAccess(x.writeAccess) {}
File &File::operator=(const File &x)
{
	Close();
	path_ = x.path_;
	return *this;
}
File::~File()
{
	Close();
}

int File::handle() const
{
	return handle_;
}

bool File::CreatePath(std::string path)
{
	const size_t delimiter = path.find_last_of("\\/");
	if (delimiter == std::string::npos)
		return false;
	// copies everything up to the '/' or '\'
	const std::string tryPath = path.substr(0, delimiter);
	if (mkdir(tryPath.c_str(), S_IRUSR | S_IWUSR | S_IXUSR) == 0)
		return true;
	if (errno == EEXIST)  // already exists
		return true;
	if (errno == ENOENT)  // no such file or directory
		return false;
	// will recursively call create path on the given path that will remove
	// one subpart at a time and return when it reaches a part where all
	// directories on the path exist and then it will create the rest.
	if (!CreatePath(tryPath))
		return false;
	if (mkdir(tryPath.c_str(), S_IRUSR | S_IWUSR | S_IXUSR) != 0)
		return false;
	return true;
}

bool File::IsAbsolutPath(const std::string &path)
{
	return !path.empty() && path[0] == '/';
}

void File::ListDirectory(const std::string &directory, std::vector<std::string> &list)
{
	// std::string dirWithSeparator(directory);
	// if (dirWithSeparator.length() > 0 &&
	// 	dirWithSeparator.back() != '\\' &&
	// 	dirWithSeparator.back() != '/')
	// 	dirWithSeparator += '/';
	DIR *dir;
	struct dirent *ent;

	list.clear();
	if ((dir = opendir(directory.c_str())) != NULL)
	{
		while ((ent = readdir(dir)) != NULL)
			if (ent->d_name[0] != '.')
				list.push_back(ent->d_name);
		// list.push_back(dirWithSeparator + ent->d_name);
		closedir(dir);
	}
}

bool File::CreateDirectory(const std::string &directory)
{
	std::string dirWithSeparator(directory);
	if (dirWithSeparator.length() &&
		dirWithSeparator[dirWithSeparator.length() - 1] != '/')
		dirWithSeparator += '/';
	return CreatePath(dirWithSeparator);
}

bool File::DeleteDirectory(const std::string &directory)
{
	//TODO
	return directory.empty();
}

bool File::CopyDirectory(const std::string &existingDirectory,
						 const std::string &newDirectory)
{
	//TODO
	return existingDirectory == newDirectory;
}

std::string File::GetWorkingDirectory()
{
	std::vector<char> workingDirectory(MAXPATHLEN);
	getcwd(&workingDirectory[0], workingDirectory.size());
	return std::string(&workingDirectory[0]);
}

void File::SetWorkingDirectory(const std::string &workingDirectory)
{
	chdir(workingDirectory.c_str());
}

uint64_t File::getSize() const
{
	struct stat st;
	stat(path_.c_str(), &st);
	return st.st_size;
	// if (handle_ < 0)
	// 	return 0;
	// struct stat st;
	// fstat(handle_, &st);
	// return st.st_size;
}
bool File::setSize(uint64_t size)
{
	// TODO
	return size;
}
uint64_t File::getPosition() const
{
	// TODO
	return -1;
}
void File::setPosition(uint64_t position)
{
	// TODO
	(void)position;
}
size_t File::Peek(Buffer &buffer, size_t numBytes, size_t offset) const
{
	// TODO
	if (numBytes == 0)
		numBytes = buffer.size();
	return Peek(&buffer[offset], numBytes);
}
size_t File::Peek(void *buffer, size_t numBytes) const
{
	// TODO
	(void)buffer;
	(void)numBytes;
	// if (handle_ < 0)
	return 0;
	// int originalPos =
}
bool File::readSelect(int fd)
{
	fd_set re;
	FD_ZERO(&re);
	FD_SET(fd, &re);
	int ret = select(fd + 1, &re, 0, 0, 0);
	if (ret > 0)
		return FD_ISSET(fd, &re);
	return false;
}
bool File::writeSelect(int fd)
{
	fd_set wr;
	FD_ZERO(&wr);
	FD_SET(fd, &wr);
	int ret = select(fd + 1, 0, &wr, 0, 0);
	if (ret > 0)
		return FD_ISSET(fd, &wr);
	return false;
}
size_t File::ReadAll(std::string &buffer)
{
	if (!readSelect(handle_))
		return 0;
	size_t totalRead = 0, fsize = getSize();
	try
	{
		char *tmp = new char[fsize + 1];
		totalRead = read(handle_, tmp, fsize);
		if (totalRead < fsize)
			std::cerr << "Error: didn't read everything...\n";
		buffer = std::string(tmp, totalRead);
		delete[] tmp;
		return totalRead;
	}
	catch (std::bad_alloc &e)
	{
		return -1;
	}
}
size_t File::ReadAll(Buffer &buffer)
{
	if (!readSelect(handle_))
		return 0;
	size_t lastRead, totalRead = 0;
	Buffer tmp(30000);
	buffer.clear();
	while ((lastRead = Read(&tmp[0], 30000)))
	{
		buffer.insert(buffer.end(), tmp.begin(), tmp.begin() + lastRead);
		totalRead += lastRead;
	}
	return totalRead;
}
size_t File::Read(Buffer &buffer, size_t numBytes, size_t offset)
{
	if (numBytes == 0)
		numBytes = buffer.size();
	return Read(&buffer[offset], numBytes);
}
size_t File::Read(void *buffer, size_t numBytes)
{
	if (handle_ < 0)
		return 0;
	if (readSelect(handle_))
	{
		int bytesRead = read(handle_, buffer, numBytes);
		return (bytesRead < 0) ? size_t(0) : size_t(bytesRead);
	}
	return 0;
}
size_t File::Write(Buffer &buffer, size_t numBytes, size_t offset)
{
	if (numBytes == 0)
		numBytes = buffer.size();
	return Write(&buffer[offset], numBytes);
}
size_t File::Write(void *buffer, size_t numBytes)
{
	if (handle_ < 0 || !writeAccess || !numBytes)
		return 0;
	if (writeSelect(handle_))
	{
		int bytesWritten = write(handle_, buffer, numBytes);
		if (bytesWritten < 0 || static_cast<size_t>(bytesWritten) < numBytes)
			std::cerr << "Error: failed to write everything into file\n";
		return (bytesWritten < 0) ? size_t(0) : size_t(bytesWritten);
	}
	return 0;
}
IFile *File::Clone() const
{
	File *clone = new File(*this);
	if (handle_ >= 0)
	{
		if (writeAccess)
		{
			clone->handle_ = open(path_.c_str(),
								  O_RDWR | O_CREAT,
								  S_IRUSR | S_IWUSR | S_IXUSR);
		}
		else
			clone->handle_ = open(path_.c_str(), O_RDONLY);
		if (clone->handle_ < 0)
		{
			delete clone;
			return 0;
		}
	}
	return clone;
}
bool File::Exists() const
{
	struct stat buf;
	if (stat(path_.c_str(), &buf) < 0)
		return false;
	return true;
}
bool File::IsDirectory() const
{
	struct stat buf;
	if (stat(path_.c_str(), &buf) == 0 && S_ISDIR(buf.st_mode))
		return true;
	return false;
}
bool File::OpenReadOnly()
{
	Close();
	handle_ = open(path_.c_str(), O_RDONLY);
	writeAccess = false;
	return handle_ >= 0;
}
void File::Close()
{
	if (handle_ < 0)
		return;
	close(handle_);
	handle_ = -1;
}
bool File::OpenReadWrite(int trunc)
{
	Close();
	handle_ = open(path_.c_str(),
				   O_RDWR | O_CREAT | trunc,
				   S_IRUSR | S_IWUSR | S_IXUSR);
	if (handle_ < 0)  // failure
	{
		if (!CreatePath(path_))
			return false;
		else
			handle_ = open(path_.c_str(),
						   O_RDWR | O_CREAT,
						   S_IRUSR | S_IWUSR | S_IXUSR);
	}
	writeAccess = true;
	return handle_ >= 0;
}

bool File::Destroy()
{
	Close();
	return (unlink(path_.c_str()) == 0);
}
bool File::Move(const std::string &path)
{
	return path.empty();
}
bool File::Copy(const std::string &destination)
{
	if (!OpenReadOnly())
		return false;
	File newFile(destination);
	if (!newFile.OpenReadWrite())
		return false;
	IFile::Buffer buffer(MAX_BLOCK_COPY_SIZE);
	while (true)
	{
		const size_t bytesToCopy = Read(buffer);
		if (bytesToCopy == 0)
			break;
		if (newFile.Write(buffer, bytesToCopy) != bytesToCopy)
			return false;
	}
	return true;
}
time_t File::GetLastModifiedTime() const
{
	struct stat buf;
	if (stat(path_.c_str(), &buf) == 0)
		return buf.st_mtime;
	return 0;
}
std::string File::GetPath() const { return path_; }
}  // namespace Utils
