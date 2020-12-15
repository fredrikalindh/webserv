#ifndef __IFILE_H__
#define __IFILE_H__

#include <stdint.h>

#include <string>
#include <vector>

namespace Utils
{
class IFile
{
public:
	typedef std::vector<uint8_t> Buffer;

	virtual ~IFile() {}

	/**
	 * This method returns the size of the file in bytes.
	*/
	virtual uint64_t getSize() const = 0;

	virtual bool setSize(uint64_t size) = 0;

	virtual uint64_t getPosition() const = 0;

	virtual void setPosition(uint64_t position) = 0;

	virtual size_t Peek(Buffer &buffer, size_t numBytes = 0, size_t offset = 0) const = 0;

	virtual size_t Peek(void *buffer, size_t numBytes) const = 0;

	/**
	 * This method reads a region of the file and advances the current 
	 * position in the file to be at the byte after the last byte read.
	 * 
	 * @param[in,out] buffer
	 * 		This will be modified to contain the bytes read from the file.
	 * @param[in] numBytes
	 * 		This is the number of bytes to read from the file.
	 * @param[in] offset
	 * 		This is the byte offset in the buffer where to store the first
	 * 		byte read from the file. 
	 * @return
	 * 		The number of bytes actually read is returned. 
	*/
	virtual size_t Read(Buffer &buffer, size_t numBytes = 0, size_t offset = 0) = 0;
	/**
	 * This method reads a region of the file and advances the current 
	 * position in the file to be at the byte after the last byte read.
	 * 
	 * @param[out] buffer
	 * 		This is where to put the bytes read from the file. 
	 * @param[in] numBytes
	 * 		This is the number of bytes to read from the file.
	 * @return
	 * 		The number of bytes actually read is returned. 
	*/
	virtual size_t Read(void *buffer, size_t numBytes) = 0;
	/**
	 * This method writes a region of the file and advances the current 
	 * position in the file to be at the byte after the last byte written.
	 * 
	 * @param[in] buffer
	 * 		This contains the bytes to write to the file.
	 * @param[in] numBytes
	 * 		This is the number of bytes to write to the file.
	 * @param[in] offset
	 * 		This is the byte offset for where to start fetch the data to 
	 * 		write in the buffer.
	 * @return
	 * 		The number of bytes actually written is returned. 
	*/
	virtual size_t Write(Buffer &buffer, size_t numBytes = 0, size_t offset = 0) = 0;
	/**
	 * This method writes a region of the file and advances the current 
	 * position in the file to be at the byte after the last byte written.
	 * 
	 * @param[in] buffer
	 * 		This contains the bytes to write to the file.
	 * @param[in] numBytes
	 * 		This is the number of bytes to write to the file.
	 * @return
	 * 		The number of bytes actually written is returned. 
	*/
	virtual size_t Write(void *buffer, size_t numBytes) = 0;
	/**
	 * This method returns a new file object which operates on the same file
	 * but has its own current file position.
	 * 
	 * @return
	 * 		A pointer to a dynamically allocated file pointer if successfull,
	 * 		otherwise 0.
	*/
	virtual IFile *Clone() const = 0;
};
}  // namespace Utils
#endif	// __IFILE_H__
