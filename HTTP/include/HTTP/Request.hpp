#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <stdint.h>

#include <map>
#include <string>

#include "Headers.hpp"
namespace Http
{
/**
 * This is a structure for storing the parsed Request data.
 * [METHOD] [PATH] *['?'QUERY] *['#'FRAGMENT] [HTTP-VERSION]
 * [HEADERS] CRLF
 * [BODY]
*/
struct Request
{
	Request();
	Request(Request const &x);
	Request &operator=(Request const &x);
	~Request();
	/**
     * This method will parse the first line of the request/
     * 
     * @return
     *      0 on success and appropriate error code on failure.
     */
	int parse_first(std::string line);
	bool set_path(std::string value);
	std::string generateRawRequest() const;
	bool setClientInfo(int socket);

	std::string ip;
	uint16_t port;
	std::string method;
	std::string path;
	std::string query;
	std::string fragment;
	Http::Headers headers;
	std::string body;
};
}  // namespace Http
#endif
