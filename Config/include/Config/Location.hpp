#ifndef __LOCATION_H__
#define __LOCATION_H__

#ifdef BONUS
#include <regex.h>
#endif
#include <Utils/Uri.hpp>
#include <map>
#include <string>
#include <vector>

#include "Parser.hpp"
#include "Token.hpp"

using namespace std;
namespace Config
{
/**
 * This class is used to store the configs in the location directives,
 * that signify a certain route. 
 */
class Location
{
	/**
     * Making Server a friend to keep the setters protected.
     */
	friend class Server;

	/**
      * This is the mode of matching on the route.
      */
	enum MatchBy
	{
		/**
           * This mode matches by prefix.
           */
		Prefix,
		/**
           * This mode matches by prefix and is preferred to regex.
           */
		Prefix_Prefered,
		/**
           * This mode requires an exact match.
           */
		Exact,
		/**
           * This mode matches by regular expressions.
           */
		Regex,
		/**
           * This mode matched by regular expressions independent on case.
           */
		Regex_CaseInsensitive,

	} matchBy_;
	/**
     * This is the uri path of the route.
     */
	string uri_;
	/**
     * This is true if the route supports autoindexing of the directories.
     */
	bool autoindex_;
	/**
     * This is the path that will be added to a given uri if matched.
     */
	string root_;
	/**
     * This is a collection of files to return if the uri is a directory.
     */
	vector<string> index_;
	/**
     * This is a collection of the http methods with a flag indicating if they are allowed.
     */
	map<string, bool> limit_except_;
	/**
     * This is the directory to which uploaded files will be added.
     */
	string upload_;
	/**
     * This is a collection of file extensions with the path to an executable cgi
     * that will handle them.
     */
	map<string, string> cgi_;
	/**
     * This is the basic authorization for this route in the form username:password.
     */
	string auth_basic_;
	/**
     * This is the max body size that a client can send.
     */
	int client_max_body_size_;
	/**
     * This is a collection of extra directives added which don't have their
     * own parameter, setter or getter but are still registered as Location 
     * directives in the token class. Accessable with 'directives(name)'.
     * 
     * @param[1] string1 
     *      This is the name of the directive such as 'cgi-bin'
     * @param[2] string2
     *      This is the value of the directive such as '/myfiles/cgi_bin'
     */
	map<string, string> directives_;

public:
	Location();
	Location(Location const &x);
	Location &operator=(Location const &x);
	virtual ~Location();

	/**
     * This method checks if the route requires an exact match and if
     * the target is an one.
     * 
     * @param[in] target
     *      This is the path given in the request.
     * @return
     *      An indication if the target given is an exact match with the uri,
     *      also means the route only supports exact matches. 
     */
	bool exactMatch(string const &target) const;
#ifdef BONUS
	bool regexMatch(string const &target) const;
#endif
	/**
     * This method checks if the prefix of the target matches the uri 
     * in the location and returns the length of the match.
     * 
     * @param[in] target
     *      This is the path given in the request.
     * @return 
     *      The length of the prefix match or 0 if location requires exact
     *      match or if the prefix did not match.
     */
	size_t matches(string const &target) const;
	/**
     * This method checks if the given method is allowed in the route.
     * 
     * @param[in]
     *      The http method to check.
     * @return
     *      An indication of if the method is allowed or not.
     */
	bool allow(string const &method) const;
	/**
     * This method returns a collection of the allowed methods for this route. 
     */
	vector<string> allowedMethods() const;
	/**
     * This method returns a collection of the allowed methods for this route. 
     */
	int maxBodySize() const;
	/**
     * This method will return the value of a directive in the directives 
     * collection if it exists.
     */
	virtual string directive(const string &name) const;
	/**
     * This method is used to check if an extension is supported
     * in the cgi.
     */
	bool hasCGI(string const &ext) const;
	/**
     * This method is used to check if an extension is supported
     * in the cgi.
     */
	vector<string> getCGI() const;
	/**
     * This method returns the path to the cgi of an extension.
     */
	string const &CGIpath(string const &ext) const;
	/**
     * This method validates if the login is valid by comparing it to 
     * the auth_basic.
     * 
     * @param[in] userInput
     *      This is the login credentials provided by the client.
     */
	bool authenticate(string const &clientInput) const;
	string const &uri() const;
	bool autoindex() const;
	string const &root() const;
	vector<string> const &index() const;
	string const &upload() const;
	string const &auth_basic() const;

	void print();  // !!! DEBUG

protected:
	/**
     * This method will take a token and redirect it to the appropriate setter.
     */
	bool dir(Parser<Token>::iterator &it);
	void location(Parser<Token>::iterator &it);
	void set_autoindex(Parser<Token>::iterator &it);
	void set_root(Parser<Token>::iterator &it);
	void set_index(Parser<Token>::iterator &it);
	void set_limit_except(Parser<Token>::iterator &it);
	void set_upload(Parser<Token>::iterator &it);
	void set_cgi(Parser<Token>::iterator &it);
	void set_auth_basic(Parser<Token>::iterator &it);
	void set_client_max_body_size(Parser<Token>::iterator &it);
	void set_directive(Parser<Token>::iterator &it);
};

}  // namespace Config
#endif	// __LOCATION_H__
