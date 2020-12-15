#ifndef __STRING_HPP__
#define __STRING_HPP__

#include <string>
#include <vector>

namespace Utils
{
class String
{
public:
	/**
	 * This function returns the given string with all letters converted to
	 * uppercase.
	 */
	static std::string toupper(const std::string &str);
	/**
	 * This function returns the given string with all letters converted to
	 * uppercase.
	 */
	static std::string tolower(const std::string &str);
	/**
	 * This function returns parts of the given string split by 'c'.
	 * 
	 * @param[in] str
	 * 		The string to split.
	 * @param[in] c
	 * 		The character to split by, default is ' '.
	 */
	static std::vector<std::string> split(const std::string &str, const char c = ' ');

	static std::string to_string(int n);
	static int to_int(const std::string &str);
};
}  // namespace Utils
#endif	// __STRING_HPP__
