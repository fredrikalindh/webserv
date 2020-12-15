#ifndef __HTTP_MESSAGEHEADERS_H__
#define __HTTP_MESSAGEHEADERS_H__

#include <iostream>
#include <string>
#include <vector>

namespace Http
{
/**
 * These are the characters that are considered whitespace and
 * should be stripped off by the Strip() function.
 */
const std::string WSP = " \t";
/**
* This is the required line terminator.
*/
const std::string CRLF = "\r\n";
/**
 * This class represents the headers of a HTTP message 
 * as defined in RFC 7230. 
 */
class Headers
{
public:
	enum State
	{
		/**
         * End of headers not yet found.
         */
		Incomplete,
		/**
         * End of headers found.
         */
		Complete,
		/**
         * Unrecoverable error, reject input.
         */
		Error
	};
	class HeaderName
	{
		std::string name_;

	public:
		HeaderName();
		HeaderName(const std::string &name);
		HeaderName(const char *name);
		HeaderName &operator=(const std::string &name);
		~HeaderName();

		/**
         * This is the equality operator, will compare the names
         * case-insensitively. 
         */
		bool operator==(const HeaderName &rhs) const;
		/**
        * This is the typecast operator to C++ string.
        *
        * @return
        *     The C++ string rendering of the header name is returned.
        */
		operator const std::string &() const;
	};

	typedef std::string HeaderValue;
	struct Header
	{
		HeaderName name;
		HeaderValue value;

		Header(const HeaderName &name, const HeaderValue &value);
	};
	typedef std::vector<Header> HeaderCollection;

	Headers();
	Headers(const Headers &x);
	Headers &operator=(const Headers &x);
	~Headers();

	/**
    * This method returns an indication of whether or not
    * the headers constructed so far have all been valid.
    *
    * @return
    *     An indication of whether or not the headers
    *     constructed so far have all been valid is returned.
    */
	bool IsValid() const;
	/**
     * This method sets the headers from a raw string message.
     * 
     * @param[in] rawMessage
     *      This is the string of the message to parse.
     * @param[out] offset
     *      This is the position to start reading from.
     * @return Total bytes accepted.
     */
	size_t parseRawMessage(const std::string &rawMessage,
						   size_t offset = 0);
	/**
     * This method returns the state;
     */
	State state() const;
	/**
     * This method returns the collection of headers.
     */
	HeaderCollection getAll() const;
	/**
     * This method checks if a header with the given name exists.
     */
	bool hasHeader(const HeaderName &name) const;
	/**
     * This method returns the value associated with the name.
	 * 
	 * @param[in] name
	 * 		The name of the header with the wanted values
	 * @param[in] delimiter
	 * 		The string to add between values, default is ",".
	 * @return
     *      The values found in the header concatenated to a single string
	 * 		separated with ", ".
     */
	HeaderValue value(const HeaderName &name,
					  std::string const &delimiter = ",") const;
	/**
     * This method returns a sequence of values associated with the name.
     * 
     * @return
     *     A collection of the values found in the header.
     */
	std::vector<HeaderValue> multiValue(const HeaderName &name) const;
	/**
     * This method returns a sequence of tokens extracted from the
     * header with the name.
	 * 
	 * @param[in] name
	 * 		The name of the header field of the wanted tokens.
     * @return
     *     A collection of the tokens found in the header.
     */
	std::vector<HeaderValue> tokens(const HeaderName &name,
									const char splitBy = ',') const;
	/**
     * This method returns an indication of whether or not the header
     * with the given name contains the given token.
     */
	bool hasToken(const HeaderName &name, const HeaderValue &token) const;
	/**
     * This method adds or replaces the header with the given name with 
     * to have the given value. 
     */
	bool setHeader(const HeaderName &name, const HeaderValue &value);
	/**
     * This method adds the header with the given name and the given value. 
     */
	bool addHeader(const HeaderName &name,
				   const HeaderValue &value);
	/**
     * This method adds the header with the given name and the value of the
     * time converted to a string.
    */
	bool addHeader(const HeaderName &name,
				   const time_t &time);
	/**
     * This method adds the header with the given name and the given values, 
     * separated by the given string. 
     */
	bool addHeader(const HeaderName &name,
				   const std::vector<HeaderValue> &values,
				   const std::string delimiter = ", ");
	/**
     * This method removes the header with the given name.
     */
	void removeHeader(const HeaderName &name);
	/**
     * This methods generates a raw string of the headers collected
     * in the object. 
     */
	std::string generateRawHeaders() const;

	void print() const;

private:
	HeaderCollection headers_;
	State state_;
};

bool operator==(const std::string &lhs, const Headers::HeaderName &rhs);

}  // namespace Http

#endif	// __MESSAGEHEADERS_H__
