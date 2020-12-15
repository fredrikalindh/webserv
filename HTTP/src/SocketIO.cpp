#include <HTTP/SocketIO.hpp>
namespace Http
{
SocketIO::SocketIO() {}
SocketIO::SocketIO(SocketIO const &x) : toRead_(x.toRead_),
										toWrite_(x.toWrite_)
{
}
SocketIO &SocketIO::operator=(SocketIO const &x)
{
	toRead_ = x.toRead_;
	toWrite_ = x.toWrite_;
	return *this;
}
SocketIO::~SocketIO() {}

size_t SocketIO::read(int socket)
{
	BufferIn::State state = toRead_[socket].read(socket);
	if (state == BufferIn::ClientDisconnected)
	{
		std::cout << "ClientDisconnected\n";
		toRead_.erase(socket);
		return false;  // return states ?
	}
	else if (state == BufferIn::Complete)
	{
		Http::Request request = toRead_[socket].request();
		if (request.setClientInfo(socket))
		{
			toWrite_[socket] = Http::handleMessage(request);
		}
		else
		{
			std::cerr << "Error: failed to retrieve client information\n";
			toWrite_[socket] = Http::handleMessage(500);
			toWrite_[socket].close = true;
		}
		toRead_.erase(socket);
	}
	else if (state == BufferIn::Error)
	{
		int ErrorCode = toRead_[socket].ErrorCode;
		ErrorCode == 0 ? ErrorCode = 400 : 0;
		toWrite_[socket] = Http::handleMessage(ErrorCode);
		toWrite_[socket].close = true;
		toRead_.erase(socket);
	}
	return true;
}
size_t SocketIO::write(int socket)
{
	if (toWrite_.count(socket))
	{
		BufferOut::State state = toWrite_[socket].write(socket);
		if (state == BufferOut::Complete || state == BufferOut::Error)
		{
			toWrite_.erase(socket);
			return state == BufferOut::Complete;
		}
	}
	return true;
}
}  // namespace Http
