#ifndef __IFILESYSTEMENTRY_H__
#define __IFILESYSTEMENTRY_H__

#include <string>
#include <vector>

#include "IFile.hpp"

namespace Utils
{
class IFileSystemEntry : public IFile
{
public:
	virtual ~IFileSystemEntry() {}

	/**
	 * This method returns an indication of whether the file on the path 
	 * exists or not.
	*/
	virtual bool Exists() const = 0;
	/**
	 * This method returns true if the path leads to a directory 
	 */
	virtual bool IsDirectory() const = 0;
	/**
	 * This method opens the file for reading, expecting it to
	 * already exist.
	 * 
	 * @return
	 * 		A flag indicating whether or not the method succeeded.
	*/
	virtual bool OpenReadOnly() = 0;
	/**
	 * This method closes the file descriptor if it's open. 
	*/
	virtual void Close() = 0;
	/**
	 * This method opens the file for reading, creating it if it 
	 * doesn't already exist.
	 * 
	 * @param[in] trunc
	 * 		Flag indicating if an existing file should be truncated or not.
	 * @return
	 * 		A flag indicating whether or not the method succeeded.
	*/
	virtual bool OpenReadWrite(int trunc) = 0;
	/**
	 * This method removes the file on the path.
	*/
	virtual bool Destroy() = 0;

	virtual bool Move(const std::string &path) = 0;

	virtual bool Copy(const std::string &destination) = 0;
	/**
	 * This method returns the time of the last modification of the file.
	*/
	virtual time_t GetLastModifiedTime() const = 0;

	virtual std::string GetPath() const = 0;
};
}  // namespace Utils

#endif	// __IFILESYSTEMENTRY_H__
