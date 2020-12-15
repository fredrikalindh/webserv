#ifndef __HTTP_BUFFERIN_HPP__
#define __HTTP_BUFFERIN_HPP__

#include <sys/socket.h>

#include <string>

#include "ChunkedBody.hpp"
#include "Request.hpp"

namespace Http
{
/**
 * This is a classed used to store messages while reading from Socket.
 */
class BufferIn
{
	static const size_t BUFFER_SIZE = 128000;
	// static const char *BUFFER;
	static char BUFFER[BUFFER_SIZE];

public:
	int ErrorCode;
	/**
     * This is used to set the state in the read process.
     */
	enum State
	{
		/**
         * Still haven't read end of first line.
         */
		ReadingStartLine,
		/**
         * Still haven't read end of headers.
         */
		ReadingHeaders,
		/**
         * Still haven't read end of body.
         */
		ReadingBody,
		/**
         * Reading Transfer-Encoding: chunked
         */
		ReadingChunked,
		/**
         * Everything has been read.
         */
		Complete,
		/**
         * Everything has been read.
         */
		ClientDisconnected,
		/**
         * Unrecoverable error, discard message.
         */
		Error
	};

	BufferIn();
	BufferIn(BufferIn const &x);
	BufferIn &operator=(BufferIn const &x);
	~BufferIn();

	State read(int fd);
	State state() const;
	Http::Request request() const;

private:
	State state_;
	size_t length_;	   // length of message
	std::string buf_;  // message to send
	size_t offset_;	   // bytes already sent or received

	Http::Request request_;
	Http::ChunkedBody chunkedBody;
};
}  // namespace Http
#endif	// HTTP_BUFFERIN_HPP
