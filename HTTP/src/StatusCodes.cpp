#include <HTTP/StatusCodes.hpp>
#include <Utils/String.hpp>
namespace Http
{
std::string StatusCodes::createString(int code)
{
	std::string codestr = Utils::String::to_string(code);
	for (int i = 0; i < 64; ++i)
		if (code == s_status_codes[i].code_)
			return codestr + " " + s_status_codes[i].message_;
	return "";
}

const StatusCodes::StatusCode StatusCodes::s_status_codes[] = {
	// 1×× Informational
	{100, "Continue"},
	{101, "Switching Protocol"},
	{102, "Processing (WebDAV)"},
	{103, "Early Hints"},
	// 2xx Success
	{200, "OK"},
	{201, "Created"},
	{202, "Accepted"},
	{203, "Non-Authoritative Information"},
	{204, "No Content"},
	{205, "Reset Content"},
	{206, "Partial Content"},
	{207, "Multi-Status"},
	{208, "Already Reported"},
	{226, "IM Used"},  //(HTTP Delta encoding)
	// 3×× Redirection
	{300, "Multiple Choice"},
	{301, "Moved Permanently"},
	{302, "Found"},
	{303, "See Other"},
	{304, "Not Modified"},
	{305, "Use Proxy"},
	{307, "Temporary Redirect"},
	{308, "Permanent Redirect"},
	// 4xx Client Error
	{400, "Bad Request"},
	{401, "Unauthorized"},
	{402, "Payment Required"},
	{403, "Forbidden"},
	{404, "Not Found"},
	{405, "Method Not Allowed"},
	{406, "Not Acceptable"},
	{407, "Proxy Authentication Required"},
	{408, "Request Timeout"},
	{409, "Conflict"},
	{410, "Gone"},
	{411, "Length Required"},
	{412, "Precondition Failed"},
	{413, "Payload Too Large"},
	{414, "URI Too Long"},
	{415, "Unsupported Media Type"},
	{416, "Requested Range Not Satisfiable"},
	{417, "Expectation Failed"},
	{418, "I'm a teapot"},
	{421, "Misdirected Request"},
	{422, "Unprocessable Entity (WebDAV)"},
	{423, "Locked (WebDAV)"},
	{424, "Failed Dependency (WebDAV)"},
	{425, "Too Early"},
	{426, "Upgrade Required"},
	{428, "Precondition Required"},
	{429, "Too Many Requests"},
	{431, "Request Header Fields Too Large"},
	{451, "Unavailable For Legal Reasons"},
	{499, "Client Closed Request"},
	// 5xx Server Error
	{500, "Internal Server Error"},
	{501, "Not Implemented"},
	{502, "Bad Gateway"},
	{503, "Service Unavailable"},
	{504, "Gateway Timeout"},
	{505, "HTTP Version Not Supported"},
	{506, "Variant Also Negotiates"},
	{507, "Insufficient Storage"},
	{508, "Loop Detected (WebDAV)"},
	{510, "Not Extended"},
	{511, "Network Authentication Required"},
	{599, "Network Connect Timeout Error"}};
}  // namespace Http
