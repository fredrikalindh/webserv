#include <gtest/gtest.h>

#include <HTTP/ChunkedBody.hpp>
#include <HTTP/Headers.hpp>

//TODO tests with not allowed tchars !("!#$%&\'*+-.^_`|~" && ALPHA && NUM)

TEST(ChunkedBodyTests, DecodeEmptyBodyOnePiece)
{
	Http::Headers headers;
	Http::ChunkedBody body(headers);
	ASSERT_EQ(5, body.decode("0\r\n\r\n"));
	ASSERT_EQ(Http::ChunkedBody::Complete, body.getState());
}
TEST(ChunkedBodyTests, DecodeEmptyBodyMultipleZeros)
{
	Http::Headers headers;
	Http::ChunkedBody body(headers);
	ASSERT_EQ(10, body.decode("000000\r\n\r\n"));
	ASSERT_EQ(Http::ChunkedBody::Complete, body.getState());
}
TEST(ChunkedBodyTests, DecodeEmptyBodyWithChunkExtensionNoValue)
{
	Http::Headers headers;
	Http::ChunkedBody body(headers);
	ASSERT_EQ(12, body.decode("000;dude\r\n\r\n"));
	ASSERT_EQ(Http::ChunkedBody::Complete, body.getState());
}
TEST(ChunkedBodyTests, DecodeEmptyBodyWithChunkExtensionUnquotedValue)
{
	Http::Headers headers;
	Http::ChunkedBody body(headers);
	ASSERT_EQ(17, body.decode("000;dude=best\r\n\r\n"));
	ASSERT_EQ(Http::ChunkedBody::Complete, body.getState());
}
TEST(ChunkedBodyTests, DecodeEmptyBodyWithChunkExtensionQuotedValue)
{
	Http::Headers headers;
	Http::ChunkedBody body(headers);
	ASSERT_EQ(36, body.decode("000;dude=\"The dude is the best \"\r\n\r\n"));
	ASSERT_EQ(Http::ChunkedBody::Complete, body.getState());
}
TEST(ChunkedBodyTests, DecodeEmptyBodyWithMultipleChunkExtensions)
{
	Http::Headers headers;
	Http::ChunkedBody body(headers);
	ASSERT_EQ(55, body.decode("000;Foo=Bar;dude=\"The dude is the best\";Spam=12345!\r\n\r\n"));
	ASSERT_EQ(Http::ChunkedBody::Complete, body.getState());
}
TEST(ChunkedBodyTests, DecodeEmptyBodyInPieces)
{
	Http::Headers headers;
	Http::ChunkedBody body(headers);
	ASSERT_EQ(1, body.decode("0"));
	ASSERT_EQ(Http::ChunkedBody::DecodingChunks, body.getState());
	ASSERT_EQ(1, body.decode("\r"));
	ASSERT_EQ(Http::ChunkedBody::DecodingChunks, body.getState());
	ASSERT_EQ(1, body.decode("\n"));
	ASSERT_EQ(Http::ChunkedBody::DecodingTrailer, body.getState());
	ASSERT_EQ(1, body.decode("\r"));
	ASSERT_EQ(Http::ChunkedBody::DecodingTrailer, body.getState());
	ASSERT_EQ(1, body.decode("\n"));
	ASSERT_EQ(Http::ChunkedBody::Complete, body.getState());
}
TEST(ChunkedBodyTests, DecodeEmptyBodyWithExtraStuffAfter)
{
	Http::Headers headers;
	Http::ChunkedBody body(headers);
	ASSERT_EQ(5, body.decode("0\r\n\r\nHello!"));
	ASSERT_EQ(Http::ChunkedBody::Complete, body.getState());
}
TEST(ChunkedBodyTests, DecodeEmptyBodyTwoPiecesSubstring)
{
	Http::Headers headers;
	Http::ChunkedBody body(headers);
	ASSERT_EQ(4, body.decode("XYZ0\r\n\r\n123", 3, 4));
	ASSERT_EQ(Http::ChunkedBody::DecodingTrailer, body.getState());
	ASSERT_EQ(1, body.decode("XYZ0\r\n\r\n123", 7, 2));
	ASSERT_EQ(Http::ChunkedBody::Complete, body.getState());
}
TEST(ChunkedBodyTests, DecodeTwoChunksOnePiece)
{
	Http::Headers headers;
	Http::ChunkedBody body(headers);
	ASSERT_EQ(15, body.decode("5\r\nHello\r\n0\r\n\r\n"));
	ASSERT_EQ(Http::ChunkedBody::Complete, body.getState());
	ASSERT_EQ("Hello", std::string(body));
}
TEST(ChunkedBodyTests, DecodeHexaSizeOnePiece)
{
	Http::Headers headers;
	Http::ChunkedBody body(headers);
	ASSERT_EQ(25, body.decode("F\r\nHello, World!!!\r\n0\r\n\r\n"));
	ASSERT_EQ(Http::ChunkedBody::Complete, body.getState());
	ASSERT_EQ("Hello, World!!!", std::string(body));
}
TEST(ChunkedBodyTests, DecodeTwoChunksTwoPieces)
{
	Http::Headers headers;
	Http::ChunkedBody body(headers);
	ASSERT_EQ(10, body.decode("5\r\nHello\r\n"));
	ASSERT_EQ(Http::ChunkedBody::DecodingChunks, body.getState());
	ASSERT_EQ(5, body.decode("0\r\n\r\n"));
	ASSERT_EQ(Http::ChunkedBody::Complete, body.getState());
	ASSERT_EQ("Hello", std::string(body));
}
TEST(ChunkedBodyTests, DecodeTwoChunksInPieces)
{
	Http::Headers headers;
	Http::ChunkedBody body(headers);
	const std::string input = "F\r\nHello, World!!!\r\n0\r\n\r\n";
	ASSERT_EQ(2, body.decode(input, 0, 2));
	ASSERT_EQ(Http::ChunkedBody::DecodingChunks, body.getState());
	ASSERT_EQ(3, body.decode(input, 2, 3));
	ASSERT_EQ(Http::ChunkedBody::ReadingChunkData, body.getState());
	ASSERT_EQ(14, body.decode(input, 5, 14));
	ASSERT_EQ(Http::ChunkedBody::ReadingChunkDelimiter, body.getState());
	ASSERT_EQ(4, body.decode(input, 19, 4));
	ASSERT_EQ(Http::ChunkedBody::DecodingTrailer, body.getState());
	ASSERT_EQ(2, body.decode(input, 23, 5));
	ASSERT_EQ(Http::ChunkedBody::Complete, body.getState());
	ASSERT_EQ("Hello, World!!!", std::string(body));
}
TEST(ChunkedBodyTests, DecodeThreeChunksOnePiece)
{
	Http::Headers headers;
	Http::ChunkedBody body(headers);
	ASSERT_EQ(37, body.decode("F\r\nHello, World!!!\r\n7\r\nIt's me\r\n0\r\n\r\n"));
	ASSERT_EQ(Http::ChunkedBody::Complete, body.getState());
	ASSERT_EQ("Hello, World!!!It's me", std::string(body));
}
TEST(ChunkedBodyTests, DecodeThreeChunksTwoPieces)
{
	Http::Headers headers;
	Http::ChunkedBody body(headers);
	ASSERT_EQ(12, body.decode("7\r\nHello, \r\n"));
	ASSERT_EQ(Http::ChunkedBody::DecodingChunks, body.getState());
	ASSERT_EQ(11, body.decode("6\r\nWorld!\r\n"));
	ASSERT_EQ(Http::ChunkedBody::DecodingChunks, body.getState());
	ASSERT_EQ(5, body.decode("0\r\n\r\n"));
	ASSERT_EQ(Http::ChunkedBody::Complete, body.getState());
	ASSERT_EQ("Hello, World!", std::string(body));
}
TEST(ChunkedBodyTests, DecodeEmptyBodyWithTrailerOnePiece)
{
	Http::Headers headers;
	Http::ChunkedBody body(headers);
	ASSERT_EQ(30, body.decode("0\r\nContent-Type: text/html\r\n\r\n"));
	ASSERT_EQ(Http::ChunkedBody::Complete, body.getState());
}

TEST(ChunkedBodyTests, DecodeSimpleNonEmptyBodyOneCharacterAtATime)
{
	Http::Headers headers;
	Http::ChunkedBody body(headers);
	const std::string input = "5\r\nHello\r\n0\r\n\r\n";
	size_t accepted = 0;
	for (size_t i = 0; i < input.length(); ++i)
	{
		accepted += body.decode(input.substr(accepted, i + 1 - accepted));
		if (i < 2)
		{
			ASSERT_EQ(Http::ChunkedBody::DecodingChunks, body.getState()) << i;
			ASSERT_EQ(i + 1, accepted);
		}
		else if (i < 7)
		{
			ASSERT_EQ(Http::ChunkedBody::ReadingChunkData, body.getState()) << i;
			ASSERT_EQ(i + 1, accepted);
		}
		else if (i < 9)
		{
			ASSERT_EQ(Http::ChunkedBody::ReadingChunkDelimiter, body.getState()) << i;
			ASSERT_EQ(i + 1, accepted);
		}
		else if (i < 12)
		{
			ASSERT_EQ(Http::ChunkedBody::DecodingChunks, body.getState()) << i;
			ASSERT_EQ(i + 1, accepted);
		}
		else if (i < 14)
		{
			ASSERT_EQ(Http::ChunkedBody::DecodingTrailer, body.getState()) << i;
			ASSERT_EQ(i + 1, accepted);	 // ! ? 13
		}
		else
		{
			ASSERT_EQ(Http::ChunkedBody::Complete, body.getState()) << i;
			ASSERT_EQ(15, accepted);
		}
	}
	ASSERT_EQ("Hello", (std::string)body);
}

TEST(ChunkedBodyTests, DecodeTwoChunkBodyOnePiece)
{
	Http::Headers headers;
	Http::ChunkedBody body(headers);
	ASSERT_EQ(28, body.decode("6\r\nHello,\r\n7\r\n World!\r\n0\r\n\r\n"));
	ASSERT_EQ(Http::ChunkedBody::Complete, body.getState());
	ASSERT_EQ("Hello, World!", (std::string)body);
}

TEST(ChunkedBodyTests, DecodeTwoChunkBodyOneCharacterAtATime)
{
	Http::Headers headers;
	Http::ChunkedBody body(headers);
	const std::string input = "6\r\nHello,\r\n7\r\n World!\r\n0\r\n\r\n";
	size_t accepted = 0;
	for (size_t i = 0; i < input.length(); ++i)
	{
		accepted += body.decode(input.substr(accepted, i + 1 - accepted));
		if (i < 2)
		{
			ASSERT_EQ(Http::ChunkedBody::DecodingChunks, body.getState()) << i;
			ASSERT_EQ(i + 1, accepted);
		}
		else if (i < 8)
		{
			ASSERT_EQ(Http::ChunkedBody::ReadingChunkData, body.getState()) << i;
			ASSERT_EQ(i + 1, accepted);
		}
		else if (i < 10)
		{
			ASSERT_EQ(Http::ChunkedBody::ReadingChunkDelimiter, body.getState()) << i;
			ASSERT_EQ(i + 1, accepted);
		}
		else if (i < 13)
		{
			ASSERT_EQ(Http::ChunkedBody::DecodingChunks, body.getState()) << i;
			ASSERT_EQ(i + 1, accepted);
		}
		else if (i < 20)
		{
			ASSERT_EQ(Http::ChunkedBody::ReadingChunkData, body.getState()) << i;
			ASSERT_EQ(i + 1, accepted);
		}
		else if (i < 22)
		{
			ASSERT_EQ(Http::ChunkedBody::ReadingChunkDelimiter, body.getState()) << i;
			ASSERT_EQ(i + 1, accepted);
		}
		else if (i < 25)
		{
			ASSERT_EQ(Http::ChunkedBody::DecodingChunks, body.getState()) << i;
			ASSERT_EQ(i + 1, accepted);
		}
		else if (i < 27)
		{
			ASSERT_EQ(Http::ChunkedBody::DecodingTrailer, body.getState()) << i;
			ASSERT_EQ(i + 1, accepted);	 // ! 26
		}
		else
		{
			ASSERT_EQ(Http::ChunkedBody::Complete, body.getState()) << i;
			ASSERT_EQ(28, accepted);
		}
	}
	ASSERT_EQ("Hello, World!", (std::string)body);
}

TEST(ChunkedBodyTests, DecodeTrailersOnePiece)
{
	Http::Headers headers;
	Http::ChunkedBody body(headers);
	EXPECT_EQ(41, body.decode("0\r\nX-Foo: Bar\r\nX-Poggers: FeelsBadMan\r\n\r\n"));
	ASSERT_EQ(Http::ChunkedBody::Complete, body.getState());
	EXPECT_EQ("", (std::string)body);
	const auto actualNumTrailers = headers.getAll().size();
	struct ExpectedTrailer
	{
		std::string name;
		std::string value;
	};
	const std::vector<ExpectedTrailer> expectedTrailers{
		{"X-Foo", "Bar"},
		{"X-Poggers", "FeelsBadMan"},
	};
	EXPECT_EQ(expectedTrailers.size(), actualNumTrailers);
	for (std::vector<ExpectedTrailer>::const_iterator it = expectedTrailers.begin(); it != expectedTrailers.end(); ++it)
	{
		EXPECT_EQ(
			it->value,
			headers.value(it->name));
	}
}

TEST(ChunkedBodyTests, DecodeTrailersOneCharacterAtATime)
{
	Http::Headers headers;
	Http::ChunkedBody body(headers);
	const std::string input = "0\r\nX-Foo: Bar\r\nX-Poggers: FeelsBadMan\r\n\r\n";
	size_t accepted = 0;
	for (size_t i = 0; i < input.length(); ++i)
	{
		accepted += body.decode(input.substr(accepted, i + 1 - accepted));
		if (i < 2)
		{
			ASSERT_EQ(Http::ChunkedBody::DecodingChunks, body.getState()) << i;
			ASSERT_EQ(i + 1, accepted) << i;
		}
		else if (i < 38)
		{
			ASSERT_EQ(Http::ChunkedBody::DecodingTrailer, body.getState()) << i;
			ASSERT_EQ(i + 1, accepted) << i;  // ! 3
		}
		else if (i < 40)
		{
			ASSERT_EQ(Http::ChunkedBody::DecodingTrailer, body.getState()) << i;
			ASSERT_EQ(i + 1, accepted) << i;  // ! 15
		}
		else
		{
			ASSERT_EQ(Http::ChunkedBody::Complete, body.getState()) << i;
			ASSERT_EQ(41, accepted) << i;
		}
	}
	const auto actualNumTrailers = headers.getAll().size();
	struct ExpectedTrailer
	{
		std::string name;
		std::string value;
	};
	const std::vector<ExpectedTrailer> expectedTrailers{
		{"X-Foo", "Bar"},
		{"X-Poggers", "FeelsBadMan"},
	};
	EXPECT_EQ(expectedTrailers.size(), actualNumTrailers);
	for (std::vector<ExpectedTrailer>::const_iterator it = expectedTrailers.begin(); it != expectedTrailers.end(); ++it)
	{
		EXPECT_EQ(
			it->value,
			headers.value(it->name));
	}
}

TEST(ChunkedBodyTests, DecodeBadChunkSizeLineNotHexdigInChunkSize)
{
	Http::Headers headers;
	Http::ChunkedBody body(headers);
	ASSERT_EQ(6, body.decode("0g\r\n\r\n"));
	ASSERT_EQ(Http::ChunkedBody::Error, body.getState());
}

TEST(ChunkedBodyTests, DecodeBadChunkSizeLineChunkSizeOverflow)
{
	Http::Headers headers;
	Http::ChunkedBody body(headers);
	ASSERT_EQ(67, body.decode("111111111111111111111111111111111111111111111111111111111111111\r\n\r\n"));
	ASSERT_EQ(Http::ChunkedBody::Error, body.getState());
}

TEST(ChunkedBodyTests, DecodeBadChunkSizeLineChunkExtensionNameFirstCharacterNotTchar)
{
	Http::Headers headers;
	Http::ChunkedBody body(headers);
	ASSERT_EQ(7, body.decode("0;@\r\n\r\n"));
	ASSERT_EQ(Http::ChunkedBody::Error, body.getState());
	headers = Http::Headers();
	body = Http::ChunkedBody(headers);
	ASSERT_EQ(7, body.decode("0;;\r\n\r\n"));
	ASSERT_EQ(Http::ChunkedBody::Error, body.getState());
	headers = Http::Headers();
	body = Http::ChunkedBody(headers);
	ASSERT_EQ(7, body.decode("0;=\r\n\r\n"));
	ASSERT_EQ(Http::ChunkedBody::Error, body.getState());
}

TEST(ChunkedBodyTests, DecodeBadChunkSizeLineChunkExtensionNameNotFirstCharacterNotTcharOrSemicolonOrEqual)
{
	Http::Headers headers;
	Http::ChunkedBody body(headers);
	ASSERT_EQ(8, body.decode("0;x@\r\n\r\n"));
	ASSERT_EQ(Http::ChunkedBody::Error, body.getState());
}

TEST(ChunkedBodyTests, DecodeBadChunkSizeLineChunkExtensionValueFirstCharacterNotQuoteOrTchar)
{
	Http::Headers headers;
	Http::ChunkedBody body(headers);
	ASSERT_EQ(9, body.decode("0;x=@\r\n\r\n"));
	ASSERT_EQ(Http::ChunkedBody::Error, body.getState());
	headers = Http::Headers();
	body = Http::ChunkedBody(headers);
	ASSERT_EQ(9, body.decode("0;x=;\r\n\r\n"));
	ASSERT_EQ(Http::ChunkedBody::Error, body.getState());
}

TEST(ChunkedBodyTests, DecodeBadChunkSizeLineChunkExtensionValueNotFirstCharacterNotTcharOrSemicolon)
{
	Http::Headers headers;
	Http::ChunkedBody body(headers);
	ASSERT_EQ(10, body.decode("0;x=y@\r\n\r\n"));
	ASSERT_EQ(Http::ChunkedBody::Error, body.getState());
}

TEST(ChunkedBodyTests, DecodeBadChunkSizeLineChunkExtensionValueQuotedStringIllegalCharacter)
{
	Http::Headers headers;
	Http::ChunkedBody body(headers);
	ASSERT_EQ(10, body.decode("0;x=\"\b\r\n\r\n"));
	ASSERT_EQ(Http::ChunkedBody::Error, body.getState());
}

TEST(ChunkedBodyTests, DecodeBadChunkSizeLineChunkExtensionValueQuotedStringBadQuotedCharacter)
{
	Http::Headers headers;
	Http::ChunkedBody body(headers);
	ASSERT_EQ(11, body.decode("0;x=\"\\\b\r\n\r\n"));
	ASSERT_EQ(Http::ChunkedBody::Error, body.getState());
}

TEST(ChunkedBodyTests, DecodeBadChunkSizeLineCharacterFollowingQuotedStringExtensionValueNotSemicolon)
{
	Http::Headers headers;
	Http::ChunkedBody body(headers);
	ASSERT_EQ(12, body.decode("0;x=\"y\"z\r\n\r\n"));
	ASSERT_EQ(Http::ChunkedBody::Error, body.getState());
}

TEST(ChunkedBodyTests, DecodeBadChunkSizeLineBadEndStateExpectingFirstExtensionNameCharacter)
{
	Http::Headers headers;
	Http::ChunkedBody body(headers);
	ASSERT_EQ(6, body.decode("0;\r\n\r\n"));
	ASSERT_EQ(Http::ChunkedBody::Error, body.getState());
}

TEST(ChunkedBodyTests, DecodeBadChunkSizeLineBadEndStateExpectingFirstExtensionValueCharacter)
{
	Http::Headers headers;
	Http::ChunkedBody body(headers);
	ASSERT_EQ(8, body.decode("0;x=\r\n\r\n"));
	ASSERT_EQ(Http::ChunkedBody::Error, body.getState());
}

TEST(ChunkedBodyTests, DecodeBadChunkSizeLineBadEndStateUnterminatedQuotedStringExtensionValueNotQuotedPair)
{
	Http::Headers headers;
	Http::ChunkedBody body(headers);
	ASSERT_EQ(9, body.decode("0;x=\"\r\n\r\n"));
	ASSERT_EQ(Http::ChunkedBody::Error, body.getState());
}

TEST(ChunkedBodyTests, DecodeBadChunkSizeLineBadEndStateUnterminatedQuotedStringExtensionValueQuotedPair)
{
	Http::Headers headers;
	Http::ChunkedBody body(headers);
	ASSERT_EQ(10, body.decode("0;x=\"\\\r\n\r\n"));
	ASSERT_EQ(Http::ChunkedBody::Error, body.getState());
}

TEST(ChunkedBodyTests, DecodeBadJunkAfterChunk)
{
	Http::Headers headers;
	Http::ChunkedBody body(headers);
	ASSERT_EQ(10, body.decode("1\r\nXjunk\r\n"));
	ASSERT_EQ(Http::ChunkedBody::Error, body.getState());
}

TEST(ChunkedBodyTests, DecodeBadTrailer)
{
	Http::Headers headers;
	Http::ChunkedBody body(headers);
	ASSERT_EQ(16, body.decode("0\r\nX-Foo Bar\r\n\r\n"));
	ASSERT_EQ(Http::ChunkedBody::Error, body.getState());
}
