#include <Config/ServerConfig.hpp>

namespace Config
{
ServerConfig::ServerConfig(Listen port, std::string const& host,
						   std::string const& target) : Server(Server::match(port, host)),
														Location(this->location_.match(target)) {}
std::string ServerConfig::directive(const std::string& name) const
{
	std::string ret = Server::directive(name);
	if (ret.empty())
		ret = Location::directive(name);
	return ret;
}
}  // namespace Config
