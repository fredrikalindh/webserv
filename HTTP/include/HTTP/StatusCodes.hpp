#ifndef STATUSCODES_HPP
#define STATUSCODES_HPP

#include <string>

namespace Http
{
/** 
 * This class is used to map status codes with their matching string description.
*/
class StatusCodes
{
public:
	/**
     * This method creates a full string from a given status code.
     * @param[in] code
     *      This is the http status code that should be matched.
     * @return 
     *      A string containing the status code and its description.
     *      ex "200 OK", "404 Not Found"
     */
	static std::string createString(int code);

private:
	struct StatusCode
	{
		int code_;
		std::string message_;
	};
	static const StatusCode s_status_codes[];
};
}  // namespace Http
#endif	// !STATUSCODES_HPP
