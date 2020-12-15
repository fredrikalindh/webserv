#ifndef __UTF8_H__
#define __UTF8_H__

#include <string>
#include <vector>
#include <stdint.h>

typedef uint32_t UnicodeCodePoint;
class Utf8
{
public:
	Utf8() : currentCharactedBeingDecoded(0),
			 numBytesRemainingToDecode(0),
			 bytesTotalToDecode(0) {}
	~Utf8() {}

	static const uint8_t UTF8_REPLACEMENT_CHAR[];
	static const UnicodeCodePoint REPLACEMENT_CHAR;
	/**
	 * This method encodes the given sequence of Unicode code points into UTF-8.
	 * 
	 * @param[in] codePoints
	 * 		These are the code points to encode.
	 * 
	 * @return 
	 * 		The UTF-8 encoding for the given code points.
	 */
	std::vector<uint8_t> encode(std::vector<UnicodeCodePoint> const &codePoints);

	/**
	 * This method decodes the given string in to a sequence of Unicode code points.
	 */
	std::vector<UnicodeCodePoint> decode(std::string const &encoding);
	/**
	 * This method decodes the given UTF-8 sequence in to a sequence of 
	 * Unicode code points.
	 */
	std::vector<UnicodeCodePoint> decode(std::vector<uint8_t> const &encoding);
	/**
	 * This method converts a given ASCII string to Unicode.
	 */
	std::vector<UnicodeCodePoint> asciiToUnicode(std::string const &ascii);

private:
	/**
	 * This is where we keep the current character that is being decoded.
	 */
	UnicodeCodePoint currentCharactedBeingDecoded;
	/**
	 * This is the number of input bytes that we still need to read in
	 * before we can fully assemble the current character that is being 
	 * decoded.
	*/
	size_t numBytesRemainingToDecode;
	/**
	 * This is the total number of bytes to decode in the current character.
	*/
	size_t bytesTotalToDecode;
};
// }  // namespace Utils
#endif	// __UTF8_H__
