#ifndef __IPARSER_H__
#define __IPARSER_H__

#include <fstream>
#include <string>
#include <vector>

/**
 * This interface is used to parse a config file into tokens.
 * Can be extended to support other file types.
 */
template <class Token>
class IParser
{
public:
    typedef typename std::vector<Token>::iterator iterator;  // ? change, doesnt have to be vector

    IParser(std::string &path);
    virtual ~IParser() {}
    virtual iterator begin() = 0;
    virtual iterator end() = 0;
};

#endif  // __IPARSER_H__
