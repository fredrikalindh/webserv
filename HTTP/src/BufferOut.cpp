#include <HTTP/BufferOut.hpp>
#include <iostream>
namespace Http
{
BufferOut::BufferOut(std::string buf) : close(false),
										state_(Incomplete),
										length_(buf.size()),
										buf_(buf),
										offset_(0) {}

BufferOut::BufferOut(BufferOut const &x) : close(x.close),
										   state_(Incomplete),
										   length_(x.length_),
										   buf_(x.buf_),
										   offset_(x.offset_) {}

BufferOut &BufferOut::operator=(BufferOut const &x)
{
	close = x.close;
	state_ = x.state_;
	length_ = x.length_;
	buf_ = x.buf_;
	offset_ = x.offset_;
	return *this;
}

BufferOut::~BufferOut() {}

BufferOut::State BufferOut::write(int fd)
{
	if (state_ == Incomplete)
	{
		int bytesOut = send(fd, buf_.c_str() + offset_, length_ - offset_, 0);
		if (bytesOut <= 0)
			return (state_ = Error);
		offset_ += bytesOut;
		if (offset_ >= length_)
			state_ = Complete;
	}
	if (state_ == Complete && close)
		state_ = Error;
	return state_;
}
}  // namespace Http
