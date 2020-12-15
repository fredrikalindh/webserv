#include <errno.h>
#include <signal.h>
#include <sys/wait.h>

#include <Config/ServerConfig.hpp>
#include <HTTP/CGI.hpp>
#include <HTTP/Server.hpp>
#include <HTTP/SocketIO.hpp>
#include <Utils/String.hpp>
#include <cstring>
#include <string>

void startWorkers(Http::Server &server)
{
	std::vector<pid_t> workers(Config::Server::WORKER_PROCESSES);
	std::cout << "Starting " << Config::Server::WORKER_PROCESSES << " servers...\n";
	for (int i = 0; i < Config::Server::WORKER_PROCESSES; ++i)
	{
		pid_t pid = fork();
		if (pid < 0)
			throw std::runtime_error(strerror(errno));
		if (pid == 0)
		{
			Http::CGI::file_ = Utils::File("cgi/cgi__body__" + Utils::String::to_string(i + 1));
			server.run(i + 1);
		}
		workers[i] = pid;
	}
	std::string input;
	while (!Http::Server::shutdown)
	{
		std::getline(std::cin, input);
		if (input == "exit")
		{
			std::cout << "Shutting down servers..." << std::endl;
			break;
		}
		else if (input == "quit")
		{
			std::cout << "Shutting down servers..." << std::endl;
			break;
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
					// std::cout << "Sending plugin signal: " << plugin << '\n';
					for (int i = 0; i < Config::Server::WORKER_PROCESSES; ++i)
						kill(workers[i], plugin);
				}
			}
		}
	}
	int sig;
	for (int i = 0; i < Config::Server::WORKER_PROCESSES; ++i)
		kill(workers[i], SIGINT);
	for (int i = 0; i < Config::Server::WORKER_PROCESSES; ++i)
		waitpid(workers[i], &sig, 0);
}

int main(int ac, char **av)
{
	Http::SocketIO io;
	// injecting a reference the IFileIO implementation SocketIO
	Http::Server server(io);
	std::string path = (ac >= 2) ? av[1] : "./conf/webserv_ubuntu.conf";
	try
	{
		Config::Server::load(path);
		// Config::ServerConfig::config_.print();	// !
		server.init(Config::Server::uniqueListens());
	}
	catch (std::exception &e)
	{
		std::cerr << "error: " << e.what() << "\n";
		return EXIT_FAILURE;
	}
	if (!Config::Server::WORKER_PROCESSES)
		server.run();
	else
		startWorkers(server);
	return EXIT_SUCCESS;
}
