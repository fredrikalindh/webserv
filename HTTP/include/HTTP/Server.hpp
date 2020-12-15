#ifndef __HTTP_SERVER_H__
#define __HTTP_SERVER_H__

#include <arpa/inet.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/select.h>
#include <unistd.h>

#include <Utils/Mutex.hpp>
#include <iostream>
#include <vector>

#include "IFileIO.hpp"
namespace Http
{
/**
 * This class is used to run a TCP Server listening on the ip_port
 * combinations given in the init function.
*/
class Server
{
	static const int STDIN = 0;

	/**
     * This is the socket fds associated with ports.
    */
	std::vector<int> port_sockets_;
	/**
     * This is the current highest fd the server has registered.
    */
	int fdmax;
	/**
     * This is the master set containing all current fds to listen on.
    */
	fd_set master_;
	// SocketIO io_;
	/**
     * This is the object responsible for all io-operations.
    */
	IFileIO &io_;

public:
	static Mutex MUTEX_SELECT;
	static bool shutdown;
	Server(IFileIO &io);
	// Server(IFileIO *io = new SocketIO());
	// Server();
	~Server();

	/**
     * This method initializes the server to listen to the ports 
     * given.
     * 
     * @param[in] ports
     *      This is the collection of ports to listen on.
    */
	void init(std::vector<uint16_t> ports);
	/**
     * This method starts the server.
    */
	void run(int worker = 0);

protected:
	/**
     * This method handles the event of a new client connecting.
     * 
     * @param[in] clientSocket
     *      The fd of the new client.
    */
	void onClientConnect(int clientSocket);
	/**
     * This method handles the event of a client disconnecting.
     * 
     * @param[in] clientSocket
     *      The fd of the client that disconnected.
    */
	void onClientDisconnect(int clientSocket);
	/**
     * This method compares the clientSocket to the port_sockets_ to 
     * see if the message is coming from there and therefore is a new 
     * Connection.
     * 
     * @param[in] clientSocket
     *      The fd to compare.
    */
	bool newConnection(int clientSocket);

private:
	int worker_;
	Server(Server const &x);
	Server &operator=(Server const &x);
	int _socket();
	bool _bind(int socketfd, struct sockaddr *address, socklen_t address_size);
	bool _listen(int socketfd);
};
}  // namespace Http

#endif	// __HTTP_SERVER_H__
