#include <gtest/gtest.h>

#include <Utils/Utf8.hpp>

// using namespace Utils;
TEST(Utf8Tests, AsciiToUnicode)
{
	Utf8 utf8;
	const std::vector<UnicodeCodePoint> expectedCodePoints{0x48, 0x65, 0x6C, 0x6C, 0x6F};
	const std::vector<UnicodeCodePoint> actualCodePoints = utf8.asciiToUnicode("Hello");
	EXPECT_EQ(expectedCodePoints, actualCodePoints);
}
TEST(Utf8Tests, EncodeAscii)
{
	Utf8 utf8;
	const std::vector<uint8_t> expectedEncoding{0x48, 0x65, 0x6C, 0x6C, 0x6F};
	std::vector<uint8_t> actualEncoding = utf8.encode(utf8.asciiToUnicode("Hello"));
	EXPECT_EQ(expectedEncoding, actualEncoding);
}
TEST(Utf8Tests, EncodeSymbols)
{
	Utf8 utf8;
	std::vector<uint8_t> expectedEncoding{0x41, 0xE2, 0x89, 0xA2, 0xCE, 0x91, 0x2E};
	std::vector<uint8_t> actualEncoding = utf8.encode({0x0041, 0x2262, 0x0391, 0x002E});  // A≢Α.
	EXPECT_EQ(expectedEncoding, actualEncoding);
	expectedEncoding = {0xE2, 0x82, 0xAC};
	actualEncoding = utf8.encode({0x20AC});	 // €
	EXPECT_EQ(expectedEncoding, actualEncoding);
}
TEST(Utf8Tests, EncodeJapanese)
{
	Utf8 utf8;
	const std::vector<uint8_t> expectedEncoding{0xE6, 0x97, 0xA5, 0xE6, 0x9C, 0xAC, 0xE8, 0xAA, 0x9E};
	std::vector<uint8_t> actualEncoding = utf8.encode({0x65E5, 0x672C, 0x8A9E});  // 日本語
	EXPECT_EQ(expectedEncoding, actualEncoding);
}
TEST(Utf8Tests, EncodeChinese)
{
	Utf8 utf8;
	const std::vector<uint8_t> expectedEncoding{0xF0, 0xA3, 0x8E, 0xB4};
	std::vector<uint8_t> actualEncoding = utf8.encode({0x233B4});	// 𣎴
	EXPECT_EQ(expectedEncoding, actualEncoding);
}
TEST(Utf8Tests, EncodeEmojiis)
{
	Utf8 utf8;
	const std::vector<uint8_t> expectedEncoding{0xF0, 0x9F, 0xA5, 0xB0, 0xF0, 0x9F, 0x98, 0x80};
	std::vector<uint8_t> actualEncoding = utf8.encode({0x1F970, 0x1F600});	 // 🥰😀
	EXPECT_EQ(expectedEncoding, actualEncoding);
}
TEST(Utf8Tests, EncodeSwedish)
{
	Utf8 utf8;
	const std::vector<uint8_t> expectedEncoding{0xC3, 0x84, 0xC3, 0x85, 0xC3, 0x96};
	std::vector<uint8_t> actualEncoding = utf8.encode({0x00C4, 0x00C5, 0x00D6});  // ÄÅÖ
	EXPECT_EQ(expectedEncoding, actualEncoding);
}
TEST(Utf8Tests, CodePointTooBig)
{
	Utf8 utf8;
	const std::vector<uint8_t> replacementCharacterEncoding{0xEF, 0xBF, 0xBD};
	EXPECT_EQ(replacementCharacterEncoding, utf8.encode({0x200000}));
	EXPECT_EQ(replacementCharacterEncoding, utf8.encode({0x110000}));
}
TEST(Utf8Tests, HighAndLowSurrogateHalves)
{
	Utf8 utf8;
	const std::vector<uint8_t> replacementCharacterEncoding{0xEF, 0xBF, 0xBD};
	EXPECT_EQ((std::vector<uint8_t>{0xED, 0x9F, 0xBF}), utf8.encode({0xD7FF}));
	EXPECT_EQ(replacementCharacterEncoding, utf8.encode({0xD800}));
	EXPECT_EQ(replacementCharacterEncoding, utf8.encode({0xD801}));
	EXPECT_EQ(replacementCharacterEncoding, utf8.encode({0xD803}));
	EXPECT_EQ(replacementCharacterEncoding, utf8.encode({0xDFFF}));
	EXPECT_EQ(replacementCharacterEncoding, utf8.encode({0xDFFE}));
	EXPECT_EQ(replacementCharacterEncoding, utf8.encode({0xDFEF}));
	EXPECT_EQ((std::vector<uint8_t>{0xEE, 0x80, 0x80}), utf8.encode({0xE000}));
}

TEST(Utf8Tests, DecodeValidSequence)
{
	struct TestVector
	{
		std::string encoding;
		std::vector<UnicodeCodePoint> decoding;
	};
	std::vector<TestVector> testVectors{
		{"𣎴", {0x233B4}},
		{"🥰😀", {0x1F970, 0x1F600}},
		{"日本語", {0x65E5, 0x672C, 0x8A9E}},
		{"A≢Α.", {0x0041, 0x2262, 0x0391, 0x002E}},
		{"€", {0x20AC}},
		{"Hello", {0x48, 0x65, 0x6C, 0x6C, 0x6F}}};
	for (std::vector<TestVector>::iterator TV = testVectors.begin(); TV != testVectors.end(); ++TV)
	{
		Utf8 utf8;
		std::vector<UnicodeCodePoint> actualDecoding = utf8.decode(TV->encoding);
		EXPECT_EQ(TV->decoding, actualDecoding);
	}
}

TEST(Utf8Tests, DecodeFromInputVector)
{
	struct TestVector
	{
		std::vector<uint8_t> encoding;
		std::vector<UnicodeCodePoint> decoding;
	};
	std::vector<TestVector> testVectors{
		{{0xF0, 0xA3, 0x8E, 0xB4}, {0x233B4}},
		{{0xF0, 0x9F, 0xA5, 0xB0, 0xF0, 0x9F, 0x98, 0x80}, {0x1F970, 0x1F600}},
		{{0xE6, 0x97, 0xA5, 0xE6, 0x9C, 0xAC, 0xE8, 0xAA, 0x9E}, {0x65E5, 0x672C, 0x8A9E}},
		{{0x41, 0xE2, 0x89, 0xA2, 0xCE, 0x91, 0x2E}, {0x0041, 0x2262, 0x0391, 0x002E}},
		{{0xE2, 0x82, 0xAC}, {0x20AC}}};
	for (std::vector<TestVector>::iterator TV = testVectors.begin(); TV != testVectors.end(); ++TV)
	{
		Utf8 utf8;
		std::vector<UnicodeCodePoint> actualDecoding = utf8.decode(TV->encoding);
		EXPECT_EQ(TV->decoding, actualDecoding);
	}
}
TEST(Utf8Tests, UnexpectedContinuationBytes)
{
	Utf8 utf8;
	std::vector<uint8_t> encodingWithUnexpectedContinuationBytes = {0x41, 0xE2, 0x89, 0xA2, 0x91, 0x2E, 0x41};
	std::vector<UnicodeCodePoint> decoding{0x0041, 0x2262, Utf8::REPLACEMENT_CHAR, 0x002E, 0x0041};
	EXPECT_EQ(decoding, utf8.decode(encodingWithUnexpectedContinuationBytes));
}
TEST(Utf8Tests, BreakInSequence)
{
	Utf8 utf8;
	std::vector<uint8_t> encodingWithUnexpectedContinuationBytes = {0x41, 0xE2, 0x89, 0xA2, 0xCE, 0x2E};
	std::vector<UnicodeCodePoint> decoding{0x0041, 0x2262, Utf8::REPLACEMENT_CHAR, 0x002E};
	EXPECT_EQ(decoding, utf8.decode(encodingWithUnexpectedContinuationBytes));
}
TEST(Utf8Tests, RejectOverLongSequence)
{
	Utf8 utf8;
	std::vector<std::vector<uint8_t> > testVectors{
		// U+2F (should only need 1 byte)
		{0xC0, 0xAF},
		{0xe0, 0x80, 0xaf},
		{0xf0, 0x80, 0x80, 0xaf},
		// One less than the minimum code point value
		// that should require this many encoded bytes
		{0xc1, 0xbf},				// U+7F (should be 1 byte)
		{0xe0, 0x9f, 0xbf},			// U+7FF (should be 2 bytes)
		{0xf0, 0x8f, 0xbf, 0xbf}};	// U+7FFF (should be 3 bytes)
	for (std::vector<std::vector<uint8_t> >::iterator TV = testVectors.begin(); TV != testVectors.end(); ++TV)
	{
		EXPECT_EQ((std::vector<UnicodeCodePoint>{Utf8::REPLACEMENT_CHAR}), utf8.decode(*TV));
	}
}
TEST(Utf8Tests, DecodeChineseInTwoSteps)
{
	Utf8 utf8;
	const std::vector<uint8_t> firstHalf{0xF0, 0xA3};
	const std::vector<uint8_t> secondHalf{0x8E, 0xB4};
	EXPECT_EQ((std::vector<UnicodeCodePoint>{}), utf8.decode(firstHalf));
	EXPECT_EQ((std::vector<UnicodeCodePoint>{0x233B4}), utf8.decode(secondHalf));
}
