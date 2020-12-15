#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <HTTP/CGI.hpp>
#include <Utils/Encoding.hpp>
#include <Utils/String.hpp>
#include <cstring>

namespace ft
{
int strlen(const char *str)
{
	int i = 0;

	while (str[i])
		i++;
	return (i);
}

char *strdup(const char *str)
{
	char *s;
	int i = 0;

	if (!(s = (char *)malloc(strlen(str) + 1)))
		return NULL;
	while (str[i])
	{
		s[i] = str[i];
		i++;
	}
	s[i] = '\0';
	return s;
}

void free_tab(char **tab)
{
	int i = 0;
	while (tab[i])
	{
		free(tab[i]);
		i++;
	}
}
}  // namespace ft
namespace Http
{
Utils::File CGI::file_("cgi/cgi__body__0");

CGI::CGI(Config::ServerConfig const &config,
		 Request const &request,
		 std::string const &ext) : config_(config),
								   request_(request),
								   ext_(ext),
								   env_(NULL)
{
	file_.OpenReadWrite();
	file_.Close();
	argv_[0] = NULL;
	argv_[1] = NULL;
	char cwd[256];
	getcwd(cwd, 256);
	cwd_ = Uri(cwd);
	Uri finalPath(config.root());
	if (request_.path.length() > config.uri().length())
		finalPath += request_.path.substr(config.uri().length());
	Uri sum = cwd_ + Uri(config_.directive("cgi-bin")) + Uri(config_.CGIpath(ext));
	cgi_path_ = sum.str();
	sum = cwd_ + finalPath;
	file_path_ = sum.str();
}

CGI::~CGI()
{
	free(argv_[0]);
	free(argv_[1]);
	file_.Destroy();
	if (env_)
	{
		ft::free_tab(env_);
		free(env_);
	}
}

bool CGI::executeCGI()
{
	if (!setCGIEnv())
		return false;
	int req[2];
	argv_[0] = ft::strdup(cgi_path_.c_str());
	argv_[1] = ft::strdup(file_path_.c_str());
	argv_[2] = NULL;
	if (pipe(req))
	{
		std::cerr << "error in pipe " << strerror(errno) << '\n';
		return false;
	}
	pid_t pid = fork();
	if (pid == 0)
	{
		int fd = open(file_.GetPath().c_str(), O_WRONLY);
		if (fd < 0)
		{
			std::cerr << "error opening file\n";
			exit(EXIT_FAILURE);
		}
		close(req[1]);
		dup2(req[0], 0);
		dup2(fd, 1);
		close(req[0]);
		close(fd);
		if (execve(argv_[0], argv_, env_))
			std::cerr << "error executing cgi " << strerror(errno) << '\n';
		exit(EXIT_FAILURE);
	}
	else if (pid < 0)
	{
		close(req[0]);
		close(req[1]);
		std::cerr << "error in fork " << strerror(errno) << '\n';
		return false;
	}
	close(req[0]);
	if (!Utils::File::writeSelect(req[1]))
	{
		std::cerr << "error in select " << strerror(errno) << '\n';
		kill(pid, SIGINT);
		return false;
	}
	write(req[1], request_.body.c_str(), request_.body.length());
	close(req[1]);
	int sig;
	if (waitpid(pid, &sig, 0) == -1 || WEXITSTATUS(sig) == EXIT_FAILURE)
	{
		std::cerr << "error in child " << strerror(errno) << '\n';
		return false;
	}
	if (!file_.OpenReadOnly())
	{
		std::cerr << "error opening " << file_.GetPath() << ", " << strerror(errno) << "\n";
		return false;
	}
	file_.ReadAll(body_);
	return true;
}

bool CGI::setCGIEnv()
{
	std::map<std::string, std::string> env;
	env["GATEWAY_INTERFACE"] = "CGI/1.1";
	env["SERVER_SOFTWARE"] = "Webserv/1.0";
	env["SERVER_PROTOCOL"] = "HTTP/1.1";
	env["SERVER_NAME"] = request_.headers.value("Host").substr(0, request_.headers.value("Host").find_first_of(':'));  // ??
	env["SERVER_PORT"] = Utils::String::to_string(request_.port);
	env["SCRIPT_NAME"] = cgi_path_;
	env["REMOTE_ADDR"] = request_.ip;
	env["REQUEST_METHOD"] = request_.method;
	env["REQUEST_URI"] = file_path_;
	env["QUERY_STRING"] = request_.query;
	env["PATH_INFO"] = file_path_;
	env["PATH_TRANSLATED"] = Uri::decode(file_path_);
	if (request_.method == "POST")
	{
		env["CONTENT_TYPE"] = request_.headers.value("Content-Type");
		env["CONTENT_LENGTH"] = Utils::String::to_string(request_.body.length());
	}
	if (!config_.auth_basic().empty())
	{
		std::string user = request_.headers.value("Authorization");
		std::string type = user.substr(0, user.find(" "));
		user = user.substr(user.find(" ") + 1);
		user = Encoding::Base64_Decode(user);
		user.erase(user.find(":"));
		env["AUTH_TYPE"] = "Basic";
		env["REMOTE_IDENT"] = user;
		env["REMOTE_USER"] = user;
	}
	if (ext_ == "php")
		env["REDIRECT_STATUS"] = "200";

	Headers::HeaderCollection head = request_.headers.getAll();
	for (Headers::HeaderCollection::const_iterator it = head.begin(); it != head.end(); it++)
	{
		// The HTTP header field name is
		// a) converted to upper case,
		// b) has alloccurrences of "-" replaced with "_" and
		// c) has "HTTP" prepended to give the meta-variable name.
		if (it->name == "Authorization" || it->name == "Content-Length" || it->name == "Content-Type")
			continue;
		std::string name("HTTP_" + Utils::String::toupper(it->name));
		std::replace(name.begin(), name.end(), '-', '_');
		env[name] = it->value;
	}
	if (!(env_ = (char **)malloc(sizeof(char *) * env.size() + 1)))
		return false;
	int i = 0;
	for (std::map<std::string, std::string>::iterator it = env.begin(); it != env.end(); i++, it++)
	{
		std::string tmp = it->first + "=" + it->second;
		env_[i] = ft::strdup(tmp.c_str());
	}
	env_[i] = NULL;
	return true;
}

std::string const &CGI::getBody() const
{
	return body_;
}

}  // namespace Http
