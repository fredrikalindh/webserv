#ifndef __HTTP_CHUNKEDBODY_H__
#define __HTTP_CHUNKEDBODY_H__

#include <HTTP/Headers.hpp>
#include <sstream>
#include <string>
#include <vector>

namespace Http
{
class ChunkedBody
{
public:
	/**
     * These are the different states that the chunked
     * body can have.
     */
	enum State
	{
		/**
         * End of chunks not yet found, decoding next chunk-size line.
         */
		DecodingChunks,
		/**
         * End of chunks not yet found, reading next chunk-data.
         */
		ReadingChunkData,
		/**
         * Read all chunk-bytes, but not yet CRLF.
         */
		ReadingChunkDelimiter,
		/**
         * End of trailer not yet found.
         */
		DecodingTrailer,
		/**
         * End of chunks and trailer found.
         */
		Complete,
		/**
         * Unrecoverable error, reject input. 
         */
		Error
	};

	ChunkedBody(Http::Headers &headers);
	ChunkedBody(ChunkedBody const &x);
	ChunkedBody &operator=(ChunkedBody const &x);
	~ChunkedBody();
	/**
     * This method continues the decoding of the chunked body,
     * passing more characters into the decoding process.
     * 
     * @note 
     *      Call getState afterwards to determine whether or not
     *      the decoding process encountered an error or completed.
     * 
     * @param[in] input
     *      This contains the characters to decode.
     * @param[in] position
     *      Starting position in input.
     * @param[in] length
     *      Max number of characters to decode. If empty all 
     *      characters from position to end will be decoded.
     * @return 
     *      The total amount of characters accepted into the 
     *      decoding process;
     */
	size_t decode(const std::string &input,
				  size_t position = 0,
				  size_t length = std::string::npos);  // ? 0 instead

	/**
     * This method returns the current state of the decoding process.
     */
	State getState() const;

	/**
     * This method returns the body as a string.
     */
	operator std::string() const;

private:
	/**
     * This is the current decoding state.
     */
	State state_;
	/**
     * If we are in the ReadingChunkData state, this is
     * the number of bytes total in the current chunk.
     */
	size_t currentChunkBytesMissing;
	/**
     * This contains the decoded body.
     */
	std::ostringstream body_;
	/**
     * This buffer is used to reassembly the chunks before decoding.
     */
	std::string reassemplyBuffer;
	/**
     * The header object from BufferIn which will parse the trailer part.
     */
	Http::Headers &headers;
};
}  // namespace Http

#endif	// __HTTP_CHUNKEDBODY_H__
