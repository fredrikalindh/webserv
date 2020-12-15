#ifndef MIMETYPES_HPP
#define MIMETYPES_HPP

#include <unistd.h>

#include <fstream>
#include <iostream>
#include <map>
#include <string>

/** 
 * This class is used to find the matching mime-type for a 
 * given extension.
*/
class Mime
{
	/**
     * This is used to store the extensions mapped to mime-types.
     */
	static std::map<std::string, std::string> type_;
	/**
     * This method initialises the type_ map from the MIME_SETUP_PATH
     */
	static void initMap();

public:
	/**
     * This method returns the type of a given extension.
     * 
     * @param[in] ext
     *      This is the extension part of a file. 
     *      For 'index.html' it is 'html'
     * @return
     *      The mime-type of the extension. 
     *      For 'html' it is 'text/html'
     */
	static std::string type(std::string ext);
};
#endif
