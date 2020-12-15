#ifndef __TOKEN_H__
#define __TOKEN_H__

#include <string>
#include <vector>

namespace Config
{
enum e_tag
{
	WORD,
	SEMI,
	O_BRACKET,
	C_BRACKET,
	SERVER,
	LOCATION,
	WORKERS,
	LISTEN,
	SERVER_NAME,
	ERROR_PAGE,
	CGI_BIN,
	CLIENT_MAX_BODY_SIZE,
	ROOT,
	UPLOAD,
	AUTOINDEX,
	INDEX,
	CGI,
	LIMIT_EXCEPT,
	AUTH_BASIC,
	PROXY
};

const std::string validDirectives[] = {"", ";", "{", "}", "server", "location", "workers", "listen", "server_name", "error_page", "cgi-bin", "client_max_body_size", "root", "upload", "autoindex", "index", "cgi", "limit_except", "auth_basic", "proxy"};

/**
 * This structure is used by the Parser to insert the values with
 * and by the Config classes to set the values in their structures. 
 */
struct Token
{
	static const size_t directivesSize = 20;
	static const size_t directivesStart = 6;
	static const size_t serverDirectivesEnd = 10;

	e_tag tag_;
	std::string value_;
	int line_idx_;

	Token();
	/**
     * This constructor takes a word and sets it to the e_tag value by matching it 
     * to the collection validDirectives and using it's index if found or otherwise
     * setting it to 0/WORD.
     */
	Token(std::string value, int line);
	Token(const std::string &value);
	Token(e_tag tag, int line);
	/**
     * Returns an indication if the token is a directive by comparing the tag
     * to the directivesStart value.
     */
	bool isDirective() const;
	/**
     * Returns an indication if the token is a directive by comparing the tag
     * to the directivesStart value and serverDirectivesEnd value.
     */
	bool isServerDirective() const;
	/**
     * Returns an indication if the token is a directive by comparing the tag
     * to the serverDirectivesEnd value.
     */
	bool isLocationDirective() const;
	/**
	 * This method compares the given value to the validDirectives
	 * and returns the appropriate e_tag.
	 * 
	 * @param[in] value
	 * 		This is a parsed string that should be tokenized.
	 */
	e_tag tag(std::string value);
};
}  // namespace Config

#endif	// __TOKEN_H__
