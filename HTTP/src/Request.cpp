#include <arpa/inet.h>

#include <HTTP/Request.hpp>
#include <Utils/String.hpp>
#include <Utils/Uri.hpp>
#include <cstring>
#include <iostream>
#include <map>
#include <sstream>

namespace Http
{
bool isValidMethod(const std::string &method_)
{
	return method_ == "GET" || method_ == "POST" ||
		   method_ == "HEAD" || method_ == "OPTIONS" ||
		   method_ == "DELETE" || method_ == "PUT" ||
		   method_ == "CONNECT" || method_ == "TRACE" ||
		   method_ == "PATCH";
}

Request::Request() {}
Request::Request(Request const &x) : ip(x.ip),
									 port(x.port),
									 method(x.method),
									 path(x.path),
									 query(x.query),
									 fragment(x.fragment),
									 headers(x.headers),
									 body(x.body) {}
Request &Request::operator=(Request const &x)
{
	ip = x.ip;
	port = x.port;
	method = x.method;
	path = x.path;
	query = x.query;
	fragment = x.fragment;
	headers = x.headers;
	body = x.body;
	return *this;
}
Request::~Request() {}

bool Request::set_path(std::string value)
{
	if (value.empty() || value[0] != '/')
		return false;
	size_t fragmentStart;
	if ((fragmentStart = value.find('#')) != std::string::npos)
	{
		fragment = value.substr(fragmentStart + 1);
		value.erase(fragmentStart);
	}
	size_t queryStart;
	if ((queryStart = value.find('?')) != std::string::npos)
	{
		query = value.substr(queryStart + 1);
		value.erase(queryStart);
	}
	path = Uri::decode(value);
	return true;
}
// Splitting first request line ex 'GET /search.html?name=fredrika&age=21 HTTP/1.1'
int Request::parse_first(std::string line)
{
	if (line.length() > 100000000)
	{
		std::cerr << "error in first: too long\n";
		return 414;
	}
	if (line.find("  ") != std::string::npos)
	{
		std::cerr << "error in first: double space\n";
		return 400;
	}
	std::vector<std::string> parsed = Utils::String::split(line);
	if (parsed.size() != 3 || parsed[2] != "HTTP/1.1")
	{
		std::cerr << "error in first: bad syntax\n";
		return 400;
	}
	if (!isValidMethod(parsed[0]))
	{
		std::cerr << "error in first: method" << method << "\n";
		return 501;
	}
	if (!set_path(parsed[1]))
	{
		std::cerr << "error in first: path\n";
		return 400;
	}
	method = parsed[0];
	return 0;
}

bool Request::setClientInfo(int socket)
{
	sockaddr_in address_;
	address_.sin_family = AF_INET;
	memset(address_.sin_zero, '\0', sizeof address_.sin_zero);
	unsigned int len = sizeof(struct sockaddr);
	int ret = getsockname(socket, reinterpret_cast<struct sockaddr *>(&address_), &len);
	if (ret < 0)
		return false;
	ip = inet_ntoa(address_.sin_addr);
	port = ntohs(address_.sin_port);
	return true;
}

std::string Request::generateRawRequest() const
{
	std::ostringstream ss;
	ss << method << ' ' << path << " HTTP/1.1"
	   << "\r\n";
	ss << headers.generateRawHeaders();
	ss << body;
	return ss.str();
}
}  // namespace Http
