#include <HTTP/ChunkedBody.hpp>
#include <limits>
namespace
{
const std::string WSP = "\r ";
/**
* This is the required line terminator.
*/
const std::string CRLF = "\r\n";
const std::string HEXA = "0123456789ABCDEF";
const std::string TCHAR = "!#$%&\'*+-.^_`|~0123456789abcdefghijklmnopqrstuvxyzABCDEFGHIJKLMNOPQRSTUVXYZ";

bool isOBStext(const char c)
{
	return c < 0;
}
bool isQDtext(const char c)
{
	return c == '\t' ||
		   c == ' ' ||
		   c == '!' ||
		   (c >= 0x23 && c <= 0x5B) ||
		   (c >= '#' && c <= '[') ||
		   (c >= 0x5D && c <= 0x7E) ||
		   (c >= ']' && c <= '~');
}
/**
 * This function decodes the given line of input as a chunk-size line.
 * 
 * Any chunk extensions are parsed but discarded.
 * 
 * @param[in] input
 *      This is the string holding the line of input.
 * @param[in] lineLength
 *      This is the length of the line of input.
 * @param[in] chunkSize
 *      This is where to store the decoded chunk-size value;
 * @return
 *      An indication of whether or not the chunk-size line was
 *      successfully parsed or not is returned.
 */
bool DecodeChunkSizeLine(
	const std::string &input,
	size_t lineLength,
	size_t &chunkSize)
{
	size_t offset, state = 0;

	chunkSize = 0;	// resetting
	for (offset = 0; input[offset] != ';' && offset < lineLength; ++offset)
	{
		size_t decimalValue;
		if ((decimalValue = HEXA.find(std::toupper(input[offset]))) == std::string::npos)
			return false;
		if ((std::numeric_limits<size_t>::max() - decimalValue) / 16 < chunkSize)  // overflow check
			return false;
		chunkSize *= 16;  // <= 4;
		chunkSize += decimalValue;
	}
	if (offset == lineLength)
		return true;
	while (++offset < lineLength)
	{
		const char c = input[offset];
		switch (state)
		{
		case 0:	 // chunk-ext: chunk-ext-name (first character)
		{
			if (TCHAR.find(c) == std::string::npos)
				return false;
			state = 1;
		}
		break;
		case 1:	 // chunk-ext: chunk-ext-name (not first character)
		{
			if (c == '=')
				state = 2;
			else if (c == ';')
				state = 0;
			else if (TCHAR.find(c) == std::string::npos)
				return false;
		}
		break;
		case 2:	 // chunk-ext: chunk-ext-val (first character)
		{
			if (c == '"')
				state = 4;
			else if (TCHAR.find(c) == std::string::npos)
				return false;
			else
				state = 3;
		}
		break;
		case 3:	 // chunk-ext: chunk-ext-val (not first character)
		{
			if (c == ';')
				state = 0;
			else if (TCHAR.find(c) == std::string::npos)
				return false;
		}
		break;
		case 4:	 // chunk-ext: chunk-ext-val (quoted string, not first character)
		{
			if (c == '"')
				state = 6;
			else if (c == '\\')
				state = 5;
			else if (!isQDtext(c) && !isOBStext(c))
				return false;
		}
		break;
		case 5:	 // chunk-ext: chunk-ext-val quoted string, second character of quoted-pair)
		{
			if (!(c == '\t' || (c >= ' ' && c <= 0x7E)))  //|| isOBStext(c))
				return false;
			state = 4;
		}
		break;
		case 6:	 // chunk-ext: chunk-ext (next character after last extension)
		{
			if (c == ';')
				state = 0;
			else
				return false;
		}
		break;
		}
	}
	return state == 1 || state == 3 || state == 6;
}

}  // namespace

namespace Http
{
// const char special_tchars[] = "!#$%&\'*+-.^_`|~";  // & ALPHA & NUM

ChunkedBody::ChunkedBody(Http::Headers &headers) : state_(DecodingChunks),
												   currentChunkBytesMissing(0),
												   headers(headers) {}
// ChunkedBody::ChunkedBody() : state_(DecodingChunks),
//                              currentChunkBytesMissing(0) {}
ChunkedBody::ChunkedBody(ChunkedBody const &x) : state_(x.state_),
												 currentChunkBytesMissing(x.currentChunkBytesMissing),
												 reassemplyBuffer(x.reassemplyBuffer),
												 headers(x.headers) {}
ChunkedBody &ChunkedBody::operator=(ChunkedBody const &x)
{
	state_ = x.state_;
	currentChunkBytesMissing = x.currentChunkBytesMissing;
	reassemplyBuffer = x.reassemplyBuffer;
	headers = x.headers;
	return *this;
}
ChunkedBody::~ChunkedBody() {}

size_t ChunkedBody::decode(const std::string &input,
						   size_t position,
						   size_t length)
{
	size_t previouslyAccepted = reassemplyBuffer.length(),
		   bytesConsumed = 0;
	reassemplyBuffer += input.substr(position, length);
	while (!reassemplyBuffer.empty() && state_ < Complete)
	{
		if (state_ == DecodingChunks)
		{
			size_t lineEnd = reassemplyBuffer.find(CRLF);
			if (lineEnd == std::string::npos)
				break;
			if (!DecodeChunkSizeLine(reassemplyBuffer,
									 lineEnd,
									 currentChunkBytesMissing))
			{
				std::cerr << "error in chunked: decoding chunk\n";
				state_ = Error;
				break;
			}
			bytesConsumed += lineEnd + CRLF.length();
			reassemplyBuffer.erase(0, lineEnd + CRLF.length());
			state_ = (currentChunkBytesMissing == 0) ? DecodingTrailer : ReadingChunkData;
		}
		if (state_ == ReadingChunkData)
		{
			size_t chunkDataToCopy = std::min(reassemplyBuffer.length(), currentChunkBytesMissing);
			if (reassemplyBuffer.length() == currentChunkBytesMissing)
				body_ << reassemplyBuffer;
			else
				body_ << reassemplyBuffer.substr(0, currentChunkBytesMissing);
			reassemplyBuffer.erase(0, chunkDataToCopy);
			bytesConsumed += chunkDataToCopy;
			currentChunkBytesMissing -= chunkDataToCopy;
			if (currentChunkBytesMissing == 0)
				state_ = ReadingChunkDelimiter;
		}
		if (state_ == ReadingChunkDelimiter)
		{
			if (reassemplyBuffer.length() < CRLF.length())
				break;
			if (reassemplyBuffer.compare(0, CRLF.length(), CRLF))
			{
				std::cerr << "error in chunked: CRLF not found\n";
				state_ = Error;
				break;
			}
			reassemplyBuffer.erase(0, CRLF.length());
			bytesConsumed += CRLF.length();
			state_ = DecodingChunks;
		}
		if (state_ == DecodingTrailer)
		{
			if (reassemplyBuffer.find(CRLF) == std::string::npos)
				break;
			size_t bytesAccepted = headers.parseRawMessage(reassemplyBuffer);
			reassemplyBuffer.erase(0, bytesAccepted);
			bytesConsumed += bytesAccepted;
			if (headers.state() == Headers::Error)
			{
				std::cerr << "error in chunked: error in trailer\n";
				state_ = Error;
				break;
			}
			if (headers.state() == Headers::Complete)
			{
				state_ = Complete;
				reassemplyBuffer.erase();
			}
		}
	}
	return reassemplyBuffer.size() + bytesConsumed - previouslyAccepted;
}

ChunkedBody::State ChunkedBody::getState() const
{
	return state_;
}
ChunkedBody::operator std::string() const
{
	return body_.str();
}

}  // namespace Http
