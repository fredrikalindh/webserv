#ifndef __HTTP_BUFFEROUT_HPP__
#define __HTTP_BUFFEROUT_HPP__

#include <sys/socket.h>

#include <string>

namespace Http
{
/**
 * This class is used to store the data to send to Socket
*/
class BufferOut
{
public:
	/**
     * This is used to set the state of the writing process.
    */
	enum State
	{
		/** 
         * Still haven't sent all bytes.
        */
		Incomplete,
		/** 
         * All bytes has been sent.
        */
		Complete,
		/** 
         * Unrecoverable error, discard message.
        */
		Error
	};
	BufferOut(std::string buf = "");
	BufferOut(BufferOut const &x);
	BufferOut &operator=(BufferOut const &x);
	~BufferOut();

	/**
     * This method will try to send the message from offset to length.
     * 
     * @param[in] fd
     *      This is the file descriptor to send to.
     * @return
     *      The current state of the buffer will be returned.
    */
	State write(int fd);

	bool close;

private:
	/**
     * This is the current state of the buffer.
    */
	State state_;
	/**
     * This is the length of the message and the total bytes to send.
    */
	size_t length_;
	/**
     * This is the message to send.
    */
	std::string buf_;
	/**
     * This is the bytes already sent.
    */
	size_t offset_;
};

}  // namespace Http

#endif	// HTTP_BUFFEROUT_HPP
