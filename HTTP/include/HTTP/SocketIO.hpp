#ifndef __SOCKETIO_H__
#define __SOCKETIO_H__

#include <map>

#include "BufferIn.hpp"
#include "BufferOut.hpp"
#include "HttpBuilder.hpp"
#include "IFileIO.hpp"
namespace Http
{
/**
 * This class is used to keep track of all the buffers while the
 * process of reading and writing is not fulfilled and sending the 
 * full Requests for Handling as well as receiving the returned 
 * Response.
*/
class SocketIO : public IFileIO
{
public:
	SocketIO();
	SocketIO(SocketIO const &x);
	SocketIO &operator=(SocketIO const &x);
	~SocketIO();
	/**
     * This method will tell a BufferIn to read from socket and if it 
     * finished forward the Request to the Http module for handling.
     * 
     * @return
     *      Returns an indication if the read was successful.
    */
	size_t read(int socket);
	/**
     * This method will tell a BufferOut to write to socket if it has
     * anything to write.
     * 
     * @return
     *      Returns an indication if the write was successful.
    */
	size_t write(int socket);

private:
	std::map<int, BufferIn> toRead_;
	std::map<int, BufferOut> toWrite_;	// ? maybe shouldn't be map?
};
}  // namespace Http

#endif	// __SOCKETIO_H__
