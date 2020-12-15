#ifndef __URI_HPP__
#define __URI_HPP__

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <stdint.h>
/**
 * This class simplifies working with Uri/Url by splitting them into a 
 * collection of subpaths thus making it easier to add them and search
 * the subpaths individually.
 */
class Uri
{
	std::vector<std::string> path_;
	bool absolute;

public:
	Uri();
	Uri(const std::string &rawPath);
	Uri(const Uri &x);
	/**
	 * This method will return the collection of subpaths. 
	 */
	std::vector<std::string> &path();
	/**
	 * This method will return the path as a string build from the
	 * collection of subpaths. 
	 */
	std::string str() const;
	Uri &operator+=(const Uri &x);
	Uri operator+(const Uri &x) const;
	/**
	 * This method will try to find the given extension in the path.
	 * @param[in] ext
	 * 		This is an extension so search for, such as 'html' or 'en-US'.
	 * @return The index of the subpath where the ext was found or -1.
	 */
	int findExt(const std::string &ext);
	/**
	 * This method will try to find the given extension in the path and 
	 * return at first success. 
	 * @param[in] exts
	 * 		This is a collection of extensions so search for, such as 
	 * 		{'html','en-US'}.
	 * @return A pair containing the index of the subpath where the 
	 * 		ext was found or -1, and a the ext found. 
	 */
	std::pair<int, std::string> findExt(const std::vector<std::string> &exts);
	/**
	 * This function will decode a given url replacing all '+' with spaces and
	 * decoding all '%' and their following hexa.
	 */
	static std::string decode(const std::string &url);
	/**
	 * This function will return all extensions in a given subpath, 
	 * meaning all parts beginning with a '.'.
	 */
	static std::vector<std::string> getExtensions(const std::string &subpath);
};

#endif	// __URI_HPP__
