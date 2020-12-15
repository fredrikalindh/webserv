#ifndef __HTTPBUILDER_H__
#define __HTTPBUILDER_H__

#include "Request.hpp"

namespace Http
{
/**
 * This function is a handle to the Http module convert a request 
 * from a given socket into a response message.
 * 
 * @param[in] clientSocket
 * 		This is the socket fd that sent the request.
 * @param[in] request 
 * 		This is the parsed Request.
 * @return
 * 		Returns the ready HTTP Response as a string.
 */
std::string handleMessage(const Request &request);
/**
 * This function is a handle to the Http module convert a request 
 * that generated an error in the parsing of the message into a response.
 * 
 * @param[in] ErrorCode 
 * 		This is the generated errorCode.
 * @return
 * 		Returns the ready HTTP Response as a string.
 */
std::string handleMessage(int ErrorCode);
}  // namespace Http
#endif	// __HTTPBUILDER_H__
