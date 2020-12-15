#include <HTTP/BufferIn.hpp>
#include <Utils/String.hpp>
namespace
{
const std::string CRLF = "\r\n";
}
namespace Http
{
char BufferIn::BUFFER[BufferIn::BUFFER_SIZE];

BufferIn::BufferIn() : ErrorCode(0),
					   state_(ReadingStartLine),
					   length_(0),
					   offset_(0),
					   chunkedBody(request_.headers) {}
BufferIn::BufferIn(BufferIn const &x) : ErrorCode(x.ErrorCode),
										state_(ReadingStartLine),
										length_(0),
										offset_(0),
										request_(x.request_),
										chunkedBody(request_.headers) {}
BufferIn &BufferIn::operator=(BufferIn const &x)
{
	state_ = x.state_;
	length_ = x.length_;
	offset_ = x.offset_;
	buf_ = x.buf_;
	return *this;
}
BufferIn::~BufferIn() {}

BufferIn::State BufferIn::read(int fd)
{
	size_t endLine;
	int bytesIn = recv(fd, BUFFER, BUFFER_SIZE, 0);
	if (bytesIn <= 0)  // 0 = clientDisconnected, -1 = error, either way we want to remove client
		return state_ = ClientDisconnected;
	buf_ += std::string(BUFFER, bytesIn);
	while (offset_ < buf_.length() && state_ < Complete)
	{
		if (state_ == ReadingStartLine)
		{
			buf_.erase(0, buf_.find_first_not_of("\r\n\t "));  // removing extra whitespace
			if ((endLine = buf_.find(CRLF, offset_)) == std::string::npos || endLine > 100000000)
			{
				if (buf_.length() > 100000000)
				{
					std::cerr << "error in request: too long uri\n";
					ErrorCode = 414;
					return state_ = Error;
				}
				break;
			}
			if ((ErrorCode = request_.parse_first(buf_.substr(0, endLine))))
			{
				std::cerr << "error in request: in first line\n";
				return state_ = Error;
			}
			offset_ = endLine + CRLF.length();
			state_ = ReadingHeaders;
		}
		if (state_ == ReadingHeaders)
		{
			offset_ += request_.headers.parseRawMessage(buf_, offset_);
			Http::Headers::State HeaderState = request_.headers.state();
			if (HeaderState == Http::Headers::Error)
			{
				std::cerr << "error in request: in headers " << HeaderState << "\n";
				return state_ = Error;
			}
			else if (HeaderState == Http::Headers::Complete)
			{
				state_ = ReadingBody;
				if (!request_.headers.hasHeader("Host"))
				{
					std::cerr << "error in request: missing host field\n";
					return state_ = Error;
				}
				if (request_.headers.hasHeader("Transfer-Encoding") &&
					request_.headers.hasToken("Transfer-Encoding", "chunked"))
				{
					state_ = ReadingChunked;
				}
				else if (request_.headers.hasHeader("Content-Length"))
				{
					if (request_.headers.value("Content-Length").find_first_not_of("0123456789") != std::string::npos)
					{
						std::cerr << "error in request: bad content-length\n";
						return state_ = Error;
					}
					try
					{
						length_ = Utils::String::to_int(request_.headers.value("Content-Length"));
						(!length_) ? state_ = Complete : 0;
					}
					catch (std::exception &e)
					{
						std::cerr << "error in request: bad content-length\n";
						return state_ = Error;
					}
				}
			}
			else
				break;
		}
		if (state_ == ReadingBody)
		{
			if (buf_.size() < length_)
				break;
			request_.body = buf_.substr(offset_, length_);
			state_ = Complete;
		}
		if (state_ == ReadingChunked)
		{
			offset_ += chunkedBody.decode(buf_, offset_);  // missing the two last ?
			Http::ChunkedBody::State ChunkState = chunkedBody.getState();
			if (ChunkState == Http::ChunkedBody::Complete)
			{
				request_.body += (std::string)chunkedBody;
				state_ = Complete;
			}
			else if (ChunkState == Http::ChunkedBody::Error)
			{
				std::cerr << "error in request: incomplete chunk\n";
				return state_ = Error;
			}
			else
				break;
		}
	}
	return state_;
}
Http::Request BufferIn::request() const
{
	return request_;
}

}  // namespace Http
