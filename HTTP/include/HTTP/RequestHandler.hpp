#ifndef __REQUESTHANDLER_H__
#define __REQUESTHANDLER_H__

#include <Config/MimeTypes.hpp>
#include <Config/ServerConfig.hpp>
#include <Utils/File.hpp>
#include <Utils/Mutex.hpp>
#include <functional>
#include <string>

#include "CGI.hpp"
#include "Request.hpp"
#include "Response.hpp"
namespace Http
{
/**
 * This class is the main class of the structure pattern for handling
 * requests. The build method will call all the protected methods where 
 * the implementations have to create method specific for handling that
 * specific request method. 
 */
class RequestHandler
{
	const Config::ServerConfig config_;
	const Request &request_;
	Response &response_;
	Utils::File target_;
	std::string ext_;

public:
	static Mutex MUTEX;
	RequestHandler(Config::ServerConfig const &config,
				   const Request &request,
				   Response &response);
	virtual ~RequestHandler();
	void build();

protected:
	void handler();
	void handlePut();
	void handlePost();
	void handleDelete();
	void handleGet();
	void handleCGI();
	bool indexRedirection();
	/**
	 * This method will verify that the request method is allowed on
	 * the matched route.
	 */
	bool method_allowed();
	/**
	 * This method will verify that the payload is not bigger than the
	 * client_max_body_size.
	 */
	bool body_size_allowed();
	/**
	 * This method will check if the route requires authentification,
	 * and if so verify that the client has the correct one.
	 */
	virtual bool authorized();
	/**
	 * This method sets the body of the response to an appropriate page if
	 * the status code shows an error. 
	 */
	void set_error_body();
	/**
	 * This method sets the headers that do not depend on the resource. 
	 */
	void set_non_specific_headers();
	/**
	 * This method checks if the path is connected to a CGI executable. 
	 */
	bool isCGI();
	/**
	 * This method searches for resources matching the target, stores them
	 * in the vector given and returns true if it finds more than one.
	 * 
	 * @param[out] matches
	 * 		This is where the files matching the target is stored.
	 * @return
	 * 		An indication of if it found more than one match.
	 */
	bool find_matches(std::vector<std::string> &matches);
	/**
	 * This method will choose between the given matches depending on the 
	 * accept headers given in the request. 
	 * @note If no such headers are given, or if there are still multiple 
	 * matches, or if no match are fulfilling the wishes it will still 
	 * choose one if the matches are not empty and set the target to the 
	 * chosen one.
	 */
	bool content_negotiation(std::vector<std::string> &matches);
};

/**
 * This function returns the quality value of an Accept header token
 * in the range from 0 to 1000. If no quality value is given it defaulted
 * to 1000.
 */
int getQuality(std::string const &token);
/**
 * This function will choose erase the matches with a content language 
 * not matching the client's preferred one, if there's at least one that
 * does match. 
 * 
 * @param[in] Tokens
 * 		This is the tokens returned from the Accept-Language header.
 * @param[in, out] matches
 * 		These are the extensions of the files matching the target.
 * 		For target 'index' and file 'index.html.en.utf-8' it includes
 * 		'.html.en.utf-8', which would match a 'en; q = 0.9' token.
 * 
 * @return The identifier of the token that the files match with, 
 * like 'en', that can be used to set the Content-Language header.
 * @retval Empty string if no match.
 */
std::string matchByLanguage(std::vector<std::string> Tokens,
							std::map<std::vector<std::string>, std::string> &matches);
// std::string matchByLanguage(std::vector<std::string> Tokens,
// 							std::vector<std::string> &matches);
/**
 * This function will choose erase the matches with a charset not matching 
 * the client's preferred one, if there's at least one that does match. 
 * 
 * @param[in] Tokens
 * 		This is the tokens returned from the Accept-Charset header.
 * @param[in, out] matches
 * 		These are the extensions of the files matching the target.
 * 		For target 'index' and file 'index.html.en.utf-8' it includes
 * 		'.html.en.utf-8', which would match a 'utf-8; q = 0.9' token.
 * 
 * @return The identifier of the token that the files match with, 
 * like 'utf-8', that can be used to set the Content-Type header comment. 
 * @retval Empty string if no match.
 */
std::string matchByCharset(std::vector<std::string> Tokens,
						   std::map<std::vector<std::string>, std::string> &matches);

}  // namespace Http
#endif	// __REQUESTHANDLER_H__
