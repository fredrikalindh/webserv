#ifndef __FILE_H__
#define __FILE_H__

#include <stdint.h>

#include <fstream>
#include <string>
#include <vector>

#include "IFileSystemEntry.hpp"
namespace Utils
{
class File : public IFileSystemEntry
{
public:
	File(std::string path);
	File(const File &x);
	File &operator=(const File &x);
	virtual ~File();

	int handle() const;
	/**
	 * This function determines if the path given is an absolute path
	 * or a relative.
	*/
	static bool IsAbsolutPath(const std::string &path);
	/**
	 * This function lists the contents of a directory.
	 * 
	 * @param[in] directory
	 * 		This is the directory to list.
	 * @param[out] list
	 * 		This is where to store the list of directory entries.
	 */
	static void ListDirectory(const std::string &directory, std::vector<std::string> &list);
	/**
	 * This function creates a directory if it doesn't already exist.
	 * 
	 * @param[in] directory
	 * 		This is the directory to create.
	 * @return 
	 * 		A flag indicating if it successfully created the directory.
	*/
	static bool CreateDirectory(const std::string &directory);
	/**
	 * This function deletes a directory and all its contents.
	 * 
	 * @param[in] directory
	 * 		This is the directory to delete.
	 * @return 
	 * 		A flag indicating if it successfully deleted the directory.
	*/
	static bool DeleteDirectory(const std::string &directory);
	/**
	 * This function copies a directory and all its contents.
	 *
     * @param[in] existingDirectory
     *     This is the directory to copy.
     *
     * @param[in] newDirectory
     *     This is the destination to which to copy the existing directory.
     *
	 * @return 
	 * 		A flag indicating if it successfully copied the directory.
	*/
	static bool CopyDirectory(const std::string &existingDirectory,
							  const std::string &newDirectory);
	/**
	 * Return the current working directory of the process.
	 */
	static std::string GetWorkingDirectory();
	/**
	 * Change the current working directory of the process.
     *
     * @param[in] workingDirectory
     *     This is the directory to set as the current working
     *     directory for the process.
	 */
	static void SetWorkingDirectory(const std::string &workingDirectory);

	static bool readSelect(int fd);
	static bool writeSelect(int fd);

	// IFile
	virtual uint64_t getSize() const;
	virtual bool setSize(uint64_t size);
	virtual uint64_t getPosition() const;
	virtual void setPosition(uint64_t position);
	virtual size_t Peek(Buffer &buffer, size_t numBytes = 0, size_t offset = 0) const;
	virtual size_t Peek(void *buffer, size_t numBytes) const;
	virtual size_t ReadAll(std::string &buffer);  // ? add in IFile
	virtual size_t ReadAll(Buffer &buffer);		  // ? add in IFile
	virtual size_t Read(Buffer &buffer, size_t numBytes = 0, size_t offset = 0);
	virtual size_t Read(void *buffer, size_t numBytes);
	virtual size_t Write(Buffer &buffer, size_t numBytes = 0, size_t offset = 0);
	virtual size_t Write(void *buffer, size_t numBytes);
	virtual IFile *Clone() const;
	// IFileSystemEntry
	virtual bool Exists() const;
	virtual bool IsDirectory() const;
	virtual bool OpenReadOnly();
	virtual void Close();
	virtual bool OpenReadWrite(int trunc = 0);
	virtual bool Destroy();
	virtual bool Move(const std::string &path);
	virtual bool Copy(const std::string &destination);
	virtual time_t GetLastModifiedTime() const;
	virtual std::string GetPath() const;

private:
	/**
	 * This is the path to the file in the file system.
	*/
	std::string path_;
	/**
	 * This is the OS handle to the underlying file.
	*/
	int handle_;
	/**
	 * This flag indicates if the the file was opened with write
	 * access or not.
	*/
	bool writeAccess;
	/**
	 * This is a helper function which creates all the directories
	 * in the given path that don't already exist.
	*/
	static bool CreatePath(std::string path);
};
}  // namespace Utils
#endif	// __FILE_H__
