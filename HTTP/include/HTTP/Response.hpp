#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <string>

#include "Headers.hpp"
#include "StatusCodes.hpp"

namespace Http
{
/**
 * This class is responsible for holding the information to send back as a
 * response to the client. 
 */
struct Response
{
	Response(int code = 200);
	// Response(Response const &x);
	// Response &operator=(Response const &x);
	~Response();

	/**
     * This method creates a raw string from the information in the class.
     */
	std::string genereateRawMessage();
	/**
     * This method fills in the body with an automated index listing the
     * files in the directory root + path.
     */
	bool listDirectory(std::string const &root, std::string const &path, std::string title = "Index");
	/**
	 * This method fills the body according to the status code. 
	 */
	void setBody();
	std::string getHtmlTagValue(const std::string &htmlTag, const std::string &endTag = ">") const;
	/**
     * This is the status code of the response.
     */
	int code;
	Http::Headers headers;
	std::string body;
};
}  // namespace Http
#endif
