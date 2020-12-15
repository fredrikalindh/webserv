#ifndef __CGI_H__
#define __CGI_H__

#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include <Config/ServerConfig.hpp>
#include <Utils/File.hpp>
#include <Utils/Uri.hpp>
#include <string>

#include "Headers.hpp"
#include "Request.hpp"

namespace Http
{
class CGI
{
public:
	static Utils::File file_;

	CGI(Config::ServerConfig const &config,
		Request const &request,
		std::string const &ext);
	~CGI();

	/**
     * We set the environnement variables for the CGI script.
     */
	bool setCGIEnv();
	/**
     * This functions forks and execute the script.
     */
	bool executeCGI();

	std::string const &getBody() const;

private:
	Config::ServerConfig const &config_;
	Request const &request_;
	std::string body_;
	std::string file_path_;
	std::string cgi_path_;
	std::string ext_;
	Uri cwd_;
	char **env_;
	char *argv_[3];
};
}  // namespace Http
#endif	// __FILE_H__
