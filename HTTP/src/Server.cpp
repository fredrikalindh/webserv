#include <errno.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h> /* See NOTES */

#include <Config/Server.hpp>
#include <HTTP/Server.hpp>
#include <cstring>
namespace Http
{
Mutex Server::MUTEX_SELECT("/sem__select");
Server::Server(IFileIO &io) : fdmax(0), io_(io) {}

Server::~Server()
{
	std::cout << "DESTRUCTOR SERVER" << std::endl;
	for (int i = 0; i < fdmax; ++i)
	{
		if (FD_ISSET(i, &master_))
		{
			close(i);
			FD_CLR(i, &master_);
		}
	}
}

void Server::init(std::vector<uint16_t> ports)
{
	sockaddr_in address;
	int socketfd = 0;

	FD_ZERO(&master_);
	address.sin_family = AF_INET;
	memset(address.sin_zero, '\0', sizeof address.sin_zero);
	for (std::vector<uint16_t>::iterator it = ports.begin();
		 it != ports.end(); ++it)
	{
		address.sin_addr.s_addr = INADDR_ANY;
		address.sin_port = *it;
		if (!(socketfd = _socket()) ||
			!_bind(socketfd, reinterpret_cast<struct sockaddr *>(&address), sizeof(address)) ||
			!_listen(socketfd))
			throw std::runtime_error(strerror(errno));
		std::cout << "binding socket on " << inet_ntoa(address.sin_addr) << ':' << ntohs(address.sin_port) << "\n";
		port_sockets_.push_back(socketfd);
		FD_SET(socketfd, &master_);
	}
	fdmax = socketfd;
}

bool Server::shutdown = false;

void InterruptHandler(int)
{
	signal(SIGINT, SIG_DFL); /* reset signal */
	std::cout << "InterruptHandler called\n";
	// std::cout << "\b\b";
	Server::shutdown = true;
}

void PluginHandler(int flags)
{
	signal(flags, SIG_DFL); /* reset signal */
	std::cout << "Plugin handler :" << flags << "\n";
	Config::Server::PLUGINS ^= flags;
}
bool handleInput()
{
	std::cout << "# handleInput\n";
	std::string input;
	getline(std::cin, input);
	if (input == "exit" || input == "quit")
	{
		Server::shutdown = true;
		return false;
	}

	size_t div = input.find_first_of("\t ");
	if (div != std::string::npos)
	{
		std::string pluginId = input.substr(0, div);
		div = input.find_first_not_of("\t ", div);
		std::string command = input.substr(div, input.find_first_of("\t; ", div) - div);
		int plugin = Config::Server::get_plugin(pluginId);
		if (plugin)
		{
			bool enable = (command == "on");
			if ((enable && !(Config::Server::PLUGINS & plugin)) ||
				(!enable && Config::Server::PLUGINS & plugin))
			{
				Config::Server::set_plugin(plugin);
				std::cout << pluginId << " is now " << (enable ? "enabled" : "disabled")
						  << "\n";
			}
		}
	}
	return true;
}

void Server::run(int worker)
{
	// struct timeval tv = {1, 0};
	worker_ = worker;
	fd_set readfds, writefds;
	FD_SET(STDIN, &master_);
	FD_ZERO(&readfds);
	FD_ZERO(&writefds);
	signal(Config::Server::DEFLATE, PluginHandler);
	signal(Config::Server::GZIP, PluginHandler);
	signal(Config::Server::UTF8, PluginHandler);
	// signal(Config::Server::UNICODE, PluginHandler);

	signal(SIGINT, InterruptHandler);
	while (!shutdown)
	{
		readfds = writefds = master_;
		if (select(fdmax + 1, &readfds, &writefds, NULL, NULL) == -1)
		{
			std::cout << "error in select\n";
			continue;
		}
		for (int clientSocket = 0; clientSocket <= fdmax; clientSocket++)
		{
			if (FD_ISSET(clientSocket, &writefds))
			{
				if (!io_.write(clientSocket))
					onClientDisconnect(clientSocket);
			}
			if (FD_ISSET(clientSocket, &readfds))
			{
				if (clientSocket == STDIN)
				{
					if (!handleInput())
						break;
				}
				else if (newConnection(clientSocket))
				{
					MUTEX_SELECT.lock();
					onClientConnect(clientSocket);
					MUTEX_SELECT.unlock();
				}
				else if (!io_.read(clientSocket))
					onClientDisconnect(clientSocket);
			}
		}
	}
	if (worker_)
		exit(0);
}

void Server::onClientDisconnect(int clientSocket)
{
	FD_CLR(clientSocket, &master_);
	close(clientSocket);
	if (fdmax == clientSocket)
		--fdmax;
}

void Server::onClientConnect(int clientSocket)
{
	int newClient = accept(clientSocket, 0, 0);
	if (newClient > 0)
	{
		std::cout << "New connection on " << worker_ << "\n";
		fcntl(newClient, F_SETFL, O_NONBLOCK);	// set it as non-blocking
		FD_SET(newClient, &master_);			// add to master_ set
		fdmax = std::max(newClient, fdmax);		// keep track of the max
	}
}

bool Server::newConnection(int clientSocket)
{
	return std::count(port_sockets_.begin(), port_sockets_.end(), clientSocket);
}

int Server::_socket()
{
	int yes = 1;
	int socketfd = socket(AF_INET, SOCK_STREAM, 0);
	if (socketfd < 0)
		return 0;
	setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
	fcntl(socketfd, F_SETFL, O_NONBLOCK);
	return socketfd;
}

bool Server::_bind(int socketfd, struct sockaddr *address, socklen_t address_size)
{
	for (int i = 1; ::bind(socketfd, address, address_size) == -1; ++i)
	{
		std::cerr << "error in bind: " << strerror(errno) << std::endl;
		if (i == 5)
			return false;
	}
	return true;
}

bool Server::_listen(int socketfd)
{
	return listen(socketfd, -1) >= 0;
}
}  // namespace Http
