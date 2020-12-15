#ifndef __SERVERCONFIG_H__
#define __SERVERCONFIG_H__

#include <stdint.h>

#include "LocationList.hpp"
#include "Server.hpp"

namespace Config
{
/**
 * This class is used as some sort of facade to expose the 
 * configuration to the other modules.
 * The config_ list is singleton thanks to private constructors
 * and the load function.
 * The ServerConfig is inheriting and by that merging matched 
 * Server and Location so they can be accessed as one.
 */
class ServerConfig : public Server, public Location
{
public:
	ServerConfig(Listen port, std::string const& host,
				 std::string const& target);
	std::string directive(const std::string& name) const;
};
}  // namespace Config
#endif	// __SERVERCONFIG_H__
