#include <arpa/inet.h>

#include <Config/ServerConfig.hpp>
#include <HTTP/HttpBuilder.hpp>
#include <HTTP/RequestHandler.hpp>
#include <HTTP/Response.hpp>
#include <HTTP/StatusCodes.hpp>
#include <Utils/Encoding.hpp>
#include <limits>
namespace Http
{
/**
 * This function forwards a request to another server on the given destination
 * and returns the processed response.
*/
std::string forwardProxy(std::string dest, const Request &request)
{
	struct timeval tv = {2, 0};
	std::cout << "forwarding to proxy on " << dest << "\n";
	uint16_t port = 80;

	if (dest.find_first_not_of("0123456789") == std::string::npos &&
		dest.length() < 6)
	{
		int ret = Utils::String::to_int(dest);
		if (ret > std::numeric_limits<uint16_t>::max())
			return handleMessage(502);
		port = ret;
	}
	else
		return handleMessage(502);

	int yes = 1;
	int socketfd = socket(AF_INET, SOCK_STREAM, 0);
	if (socketfd < 0)
		return handleMessage(502);
	setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
	fcntl(socketfd, F_SETFL, O_NONBLOCK);

	sockaddr_in address;
	address.sin_family = AF_INET;
	memset(address.sin_zero, '\0', sizeof address.sin_zero);
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);
	if (!connect(socketfd, reinterpret_cast<struct sockaddr *>(&address), sizeof(address)))
		return handleMessage(500);
	std::string rawRequest = request.generateRawRequest();
	fd_set master, iofd;
	FD_ZERO(&master);
	FD_ZERO(&iofd);
	FD_SET(socketfd, &master);
	int totalBytes = 0;
	while (static_cast<size_t>(totalBytes) < rawRequest.size())
	{
		iofd = master;
		if (select(socketfd + 1, 0, &iofd, 0, &tv) <= 0)
			return handleMessage(504);
		int bytesOut = send(socketfd, rawRequest.c_str() + totalBytes, rawRequest.size() - totalBytes, 0);
		if (bytesOut < 0)
			return handleMessage(502);
		totalBytes += bytesOut;
	}
	char buffer[128000];
	std::string rawResponse;
	while (true)
	{
		iofd = master;
		if (select(socketfd + 1, &iofd, 0, 0, &tv) <= 0)
			return handleMessage(504);
		int bytesIn = recv(socketfd, buffer, sizeof buffer, 0);
		if (bytesIn < 0)
			return handleMessage(502);
		if (bytesIn == 0)
			break;
		rawResponse += std::string(buffer, bytesIn);
		if (bytesIn < 128000)
			break;
	}
	close(socketfd);
	return rawResponse;
}

/**
 * This function applys the prefered encoding as indicated by the client 
 * to the body of the response and sets the Content-Encoding header 
 * accordingly.
 */
void applyEncodings(const std::vector<std::string> &acceptableEncodings, Response &response)
{
	for (std::vector<std::string>::const_iterator encoding = acceptableEncodings.begin();
		 encoding != acceptableEncodings.end(); ++encoding)
	{
		std::vector<uint8_t> compressed;
		if (*encoding == "gzip" &&
			Config::Server::PLUGINS & Config::Server::GZIP)
		{
			if (Encoding::Deflate(response.body,
								  compressed,
								  Encoding::GZIP) == Encoding::SUCCESS)
			{
				response.body = std::string(compressed.begin(), compressed.end());
				response.headers.setHeader("Content-Encoding", "gzip");
				response.headers.removeHeader("Content-Length");
				// response.headers.setHeader("Content-Length", Utils::String::to_string(response.body.length()));
				break;
			}
		}
		if (*encoding == "deflate" &&
			Config::Server::PLUGINS & Config::Server::DEFLATE)
		{
			if (Encoding::Deflate(response.body,
								  compressed,
								  Encoding::DEFLATE) == Encoding::SUCCESS)
			{
				response.body = std::string(compressed.begin(), compressed.end());
				response.headers.setHeader("Content-Encoding", "deflate");
				response.headers.removeHeader("Content-Length");
				break;
			}
		}
	}
}

std::string handleMessage(const Request &request)
{
	Response response;
	Config::ServerConfig config(request.port, request.headers.value("Host"), request.path);
	if (!config.directive("proxy").empty())
		return forwardProxy(config.directive("proxy"), request);
	RequestHandler handleRequest(config, request, response);
	handleRequest.build();
	if (request.headers.hasHeader("Accept-Encoding") &&
		Config::Server::PLUGINS)
		applyEncodings(request.headers.tokens("Accept-Encoding"), response);
	return response.genereateRawMessage();
}

std::string handleMessage(int ErrorCode)
{
	Response response(ErrorCode);
	response.setBody();
	return response.genereateRawMessage();
}
}  // namespace Http
