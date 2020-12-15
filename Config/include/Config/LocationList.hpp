#ifndef __LOCATIONLIST_H__
#define __LOCATIONLIST_H__

#include <vector>

#include "Location.hpp"
namespace Config
{
/**
 * This class is storing the collection of routes in the config, 
 * making matching easier.
 */
class LocationList
{
	std::vector<Location> location_;

public:
	LocationList();
	LocationList(LocationList const &x);
	LocationList &operator=(LocationList const &x);
	~LocationList();

	/**
     * This method will match the target with the appopriate Location object.
     * 
     * @param[in] target
     *      This is the target path from the request.
     * @return
     *      A location object with the best matching uri among the locations
     *      is the list.
     */
	Location const &match(std::string const &target) const;
	/**
     * This method adds a Location to the collection.
     * 
     * @return 
     *      A reference to the added location. 
     */
	Location &add(Location toAdd);
	/**
     * This method returns an indication if the locations are empty or not.
     */
	bool empty();

	/**
     * ! Debugging method
     */
	void print();
};
}  // namespace Config
#endif	// __LOCATIONLIST_H__
