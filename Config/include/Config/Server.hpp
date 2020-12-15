#ifndef __SERVER_H__
#define __SERVER_H__

#include <stdint.h>

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "LocationList.hpp"
#include "Parser.hpp"
#include "Token.hpp"

using namespace ::std;

typedef uint16_t Listen;

namespace Config
{
/**
 * This class is used to store the configs for a server directive.
 */
class Server
{
	friend class ServerConfig;

protected:
	/**
      * These are the configurations for the server given in the config file.
      */
	static std::vector<Server> configs_;
	/**
     * This method is used to find a Server Config matching the requests ip-port
     * combination and, if necessary, Host header field value.
     */
	static Server const &match(Listen toMatch, std::string const &hostField);
	/**
     * This method is used if there were several matches on the ip-port 
     * to select Server Config by Host header field value.
     */
	static int matchByHost(std::vector<int> matches, std::string hostName);
	/**
      * This method adds a new server to the collection of configs.
      */
	static Server &add(Server toAdd);

public:
	static int WORKER_PROCESSES;
	static u_int16_t PLUGINS;
	enum AvailablePlugins
	{
		UTF8 = 0x04,	 //0b0000000000000100
		DEFLATE = 0x08,	 //0b0000000000001000
		GZIP = 0x10,	 //0b0000000000010000
		UNICODE = 0x22,	 //0b0000000000100000
	};
	static int get_plugin(const std::string &plugin);
	static int set_plugin(const std::string &plugin);
	static void set_plugin(const u_int16_t plugins);
	Server();
	Server(Server const &x);
	Server &operator=(Server const &x);
	virtual ~Server();

	/**
     * This method loads a file on the given path to the parser and then 
     * initializes the different Config objects bases on the parsed output.
     */
	static void load(std::string &path);
	/**
     * This method returns the unique ip-port combinations listed in the Server
     * objects in the collection.
     */
	static std::vector<uint16_t> uniqueListens();

	static void printAll()	// !!! DEBUG
	{
		std::cout << "##### configured " << configs_.size() << " servers\n\n";
		for (std::vector<Server>::iterator it = configs_.begin(); it != configs_.end(); ++it)
			it->print();
	}

	void print();  // !!! DEBUG

	/**
     * This method will return an indication if the given hostName
     * is an exact match with one of the set server_names.
     * 
     * @param[in] hostName
     *      This is the value of the Host header field in the request. 
     * @return 
     *      True if the hostName has an exact match in the server_name field.
     */
	bool exactMatch(string const &hostName) const;
	/**
     * This method will check if the hostName is matching any of the server
     * names with a leading or trailing wildcard ('*').
     * 
     * @param[in] hostName
     *      This is the value of the Host header field in the request. 
     * @return 
     *      If no match it will return 0 otherwise it will return the size of 
     *      the match, positive for a leading and negative for a trailing.
     */
	int wildcardMatch(string const &hostName) const;
	/**
      * This returns the ip-port combination the server listens on.
      */
	Listen const &listen() const;
	/**
     * This method returns an indication if the given code has a registered 
     * errorPage.
     * 
     * @param[in] code
     *      An HTTP status code. 
     */
	bool errorPageExists(int code) const;
	/**
     * This method returns the path registered for the errorPage of a given code.
     */
	string const &errorPage(int code) const;
	/**
     * This method will return the value of a directive in the directives 
     * collection if it exists.
     */
	virtual string directive(const string &name) const;

protected:
	Config::LocationList location_;
	/**
     * This method will take a token and redirect it to the appropriate setter.
     */
	bool dir(Parser<Token>::iterator &it);
	/**
     * This method is traversing the tokens from server { to closing bracket, 
     * setting the values listed inside. 
     */
	void server(Parser<Token>::iterator &it);
	/**
     * This is the object where we store the ip-port combination the server
     * listens on.
     */
	Listen listen_;	 // ? in nginx multiple but maybe we settle for one?
	/**
     * This is the collection of server names registered for the server.
     */
	vector<string> server_name_;
	/**
     * This is mapping paths to error codes that should be sent in case of an error.
     */
	map<int, string> error_page_;
	/**
     * This is a collection of extra directives added. 
     */
	map<string, string> directives_;

	void set_listen(Parser<Token>::iterator &it);
	void set_server_name(Parser<Token>::iterator &it);
	void set_location(Parser<Token>::iterator &it);
	void set_error_page(Parser<Token>::iterator &it);
	void set_directive(Parser<Token>::iterator &it);
};

void set_workers(Parser<Token>::iterator &it);
bool set_plugins(Parser<Token>::iterator &it);
}  // namespace Config
#endif	// __SERVER_H__
