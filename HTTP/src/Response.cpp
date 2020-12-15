#include <sys/time.h>
#include <unistd.h>

#include <HTTP/Response.hpp>
#include <Utils/File.hpp>
#include <Utils/String.hpp>
#include <iostream>
#include <sstream>
#include <vector>
namespace Http
{
Response::Response(int code) : code(code) {}

Response::~Response() {}

bool Response::listDirectory(std::string const &directory,
							 std::string const &requestTarget,
							 std::string title)
{
	body = "<h1>" + title + "</h1>\n";
	std::vector<std::string> entriesInDirectory;
	std::string fileName;

	Utils::File::ListDirectory(directory, entriesInDirectory);
	for (std::vector<std::string>::iterator it = entriesInDirectory.begin(); it != entriesInDirectory.end(); ++it)
		body += "<p><a href=\"" + requestTarget + "/" + *it + "\">" + *it + "</a></p>\n";
	headers.setHeader("Content-Length", Utils::String::to_string(body.length()));
	return true;
}

std::string Response::getHtmlTagValue(const std::string &htmlTag, const std::string &endTag) const
{
	std::string value;
	if (body.length() && htmlTag.length())
	{
		size_t startTag = body.find(htmlTag) + htmlTag.length();
		size_t end = body.find(endTag, startTag);
		if (startTag == std::string::npos || end == std::string::npos)
			return value;
		value = body.substr(startTag, end - startTag);
	}
	return value;
}

void Response::setBody()
{
	std::string status = StatusCodes::createString(code);
	headers.setHeader("Content-Type", "text/html");
	body = std::string(
		"<html>\n"
		"<head><title>" +
		status +
		"</title></head>\n"
		"<body>\n"
		"<center><h1>" +
		status +
		"</h1></center>\n"
		"<hr><center>webserv/42</center>\n"
		"</body>\n"
		"</html>\n");
}

std::string Response::genereateRawMessage()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	headers.addHeader("Date", tv.tv_sec);
	headers.addHeader("Server", "42Webserv");
	std::ostringstream ss;
	std::string status = StatusCodes::createString(code);
	if (status.empty())
		status = "500 Internal Server Error";
	ss << "HTTP/1.1 " << status << "\r\n";
	if (!headers.hasHeader("Content-Length"))
		ss << "Content-Length: " << body.length() << "\r\n";
	ss << headers.generateRawHeaders();
	ss << body;
	return ss.str();
}

}  // namespace Http
