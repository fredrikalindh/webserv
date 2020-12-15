#ifndef __IFILEIO_H__
#define __IFILEIO_H__

#include <unistd.h>

namespace Http
{
class IFileIO
{
public:
	enum State
	{
		Incomplete,

		Complete,

		ClientDisconnected,

		Error
	};
	virtual ~IFileIO() {}
	virtual size_t write(int fd) = 0;
	virtual size_t read(int fd) = 0;
};
}  // namespace Http
#endif
