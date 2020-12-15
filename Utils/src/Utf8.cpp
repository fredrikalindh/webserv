#include <Utils/Utf8.hpp>
namespace
{
/**
 * This computes the logarithm base 2 of the given integer.
 * 
 * @param[in] integer
 * 
 * @return 
 * 		THe logarithm of the given integer in base 2.
 */
template <typename I>
size_t log2n(I nbr)
{
	size_t log = 0;
	while (nbr > 0)
	{
		++log;
		// nbr >>= 1;
		nbr /= 2;
	}
	return log;
}
}  // namespace

// namespace Utils
// {
/**
 * This is the UTF-8 Replacement char: �. 
 */
const uint8_t Utf8::UTF8_REPLACEMENT_CHAR[] = {0xEF, 0xBF, 0xBD};
const UnicodeCodePoint Utf8::REPLACEMENT_CHAR = 0xFFFD;
/**
 * This is the last legal code point in Unicode.
 */
const UnicodeCodePoint LAST_LEGAL_UNICODE_CODE_POINT = 0x10FFFF;
/**
 * Surrogates are characters in the Unicode range U+D800—U+DFFF 
 * (2,048 code points), used by UTF-16 and their UTF-8 encoding must be
 * treated as an invalid byte sequence.
 */
const UnicodeCodePoint FIRST_SURROGATE = 0xD800;
const UnicodeCodePoint LAST_SURROGATE = 0xDFFF;

std::vector<UnicodeCodePoint> Utf8::asciiToUnicode(std::string const &ascii)
{
	return std::vector<UnicodeCodePoint>(ascii.begin(), ascii.end());
}

std::vector<uint8_t> Utf8::encode(std::vector<UnicodeCodePoint> const &codePoints)
{
	std::vector<uint8_t> encoding;
	for (std::vector<UnicodeCodePoint>::const_iterator it = codePoints.begin();
		 it != codePoints.end(); ++it)
	{
		size_t numBits = log2n(*it);
		if (numBits <= 7)
			encoding.push_back(static_cast<UnicodeCodePoint>(*it & 0x7F));
		else if (numBits <= 11)
		{
			encoding.push_back(static_cast<UnicodeCodePoint>(((*it >> 6) & 0x1F) + 0xC0));
			encoding.push_back(static_cast<UnicodeCodePoint>((*it & 0x3F) + 0x80));
		}
		else if (numBits <= 16 && (*it < FIRST_SURROGATE || *it > LAST_SURROGATE))
		{
			encoding.push_back(static_cast<UnicodeCodePoint>(((*it >> 12) & 0x0F) + 0xE0));
			encoding.push_back(static_cast<UnicodeCodePoint>(((*it >> 6) & 0x3F) + 0x80));
			encoding.push_back(static_cast<UnicodeCodePoint>((*it & 0x3F) + 0x80));
		}
		else if (numBits > 16 && numBits <= 21 && *it <= LAST_LEGAL_UNICODE_CODE_POINT)
		{
			encoding.push_back(static_cast<UnicodeCodePoint>(((*it >> 18) & 0x07) + 0xF0));
			encoding.push_back(static_cast<UnicodeCodePoint>(((*it >> 12) & 0x3F) + 0x80));
			encoding.push_back(static_cast<UnicodeCodePoint>(((*it >> 6) & 0x3F) + 0x80));
			encoding.push_back(static_cast<UnicodeCodePoint>((*it & 0x3F) + 0x80));
		}
		else
		{
			encoding.insert(encoding.end(),
							UTF8_REPLACEMENT_CHAR,
							UTF8_REPLACEMENT_CHAR + 3);
		}
	}
	return encoding;
}

std::vector<UnicodeCodePoint> Utf8::decode(std::string const &encoding)
{
	return decode(std::vector<uint8_t>(encoding.begin(), encoding.end()));
}

std::vector<UnicodeCodePoint> Utf8::decode(std::vector<uint8_t> const &encoding)
{
	std::vector<UnicodeCodePoint> decoded;
	for (std::vector<uint8_t>::const_iterator octet = encoding.begin(); octet != encoding.end(); ++octet)
	{
		if (numBytesRemainingToDecode == 0)
		{
			if ((*octet & 0x80) == 0)  // ascii
			{
				decoded.push_back(*octet);
			}
			else if ((*octet & 0xE0) == 0xC0)
			{
				numBytesRemainingToDecode = 1;
				currentCharactedBeingDecoded = (*octet & 0x1F);
			}
			else if ((*octet & 0xF0) == 0xE0)
			{
				numBytesRemainingToDecode = 2;
				currentCharactedBeingDecoded = (*octet & 0x0F);
			}
			else if ((*octet & 0xF8) == 0xF0)
			{
				numBytesRemainingToDecode = 3;
				currentCharactedBeingDecoded = (*octet & 0x07);
			}
			else
			{
				decoded.push_back(REPLACEMENT_CHAR);
			}
			bytesTotalToDecode = numBytesRemainingToDecode + 1;
		}
		// missing Continuaion Byte
		else if ((*octet & 0xC0) != 0x80)
		{
			decoded.push_back(REPLACEMENT_CHAR);
			numBytesRemainingToDecode = 0;
			std::vector<UnicodeCodePoint> nextCodePoints = decode(std::vector<uint8_t>(1, *octet));
			decoded.insert(decoded.end(), nextCodePoints.begin(), nextCodePoints.end());
		}
		else
		{
			currentCharactedBeingDecoded <<= 6;
			currentCharactedBeingDecoded += (*octet & 0x3F);
			if (--numBytesRemainingToDecode == 0)
			{
				// check if it's encoded in unnecessarily many bytes
				if ((bytesTotalToDecode == 2 && currentCharactedBeingDecoded < 0x00080) ||
					(bytesTotalToDecode == 3 && currentCharactedBeingDecoded < 0x00800) ||
					(bytesTotalToDecode == 4 && currentCharactedBeingDecoded < 0x10000))
					decoded.push_back(REPLACEMENT_CHAR);
				else
					decoded.push_back(currentCharactedBeingDecoded);
				currentCharactedBeingDecoded = 0;
			}
		}
	}
	return decoded;
}

// }  // namespace Utils
