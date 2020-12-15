#include <gtest/gtest.h>

#include <HTTP/Headers.hpp>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

TEST(HeadersTests, EmptyHeaders)
{
	Http::Headers headers;
	EXPECT_EQ((size_t)2, headers.parseRawMessage("\r\n"));
	EXPECT_EQ(Http::Headers::Complete, headers.state());
}
TEST(HeadersTests, EmptyHeadersWithOffset)
{
	Http::Headers headers;
	EXPECT_EQ((size_t)2, headers.parseRawMessage("Hejhejhej\r\n", 9));
	EXPECT_EQ(Http::Headers::Complete, headers.state());
}

TEST(HeadersTests, HeaderNameEquivalency)
{
	typedef Http::Headers::HeaderName lhs;
	typedef Http::Headers::HeaderName rhs;

	EXPECT_TRUE(lhs("hello") == rhs("hello"));
	EXPECT_TRUE(lhs("Hello") == rhs("hello"));
	EXPECT_FALSE(lhs("jello") == rhs("hello"));
	EXPECT_FALSE(lhs("hello") == rhs("hell"));
}

// TEST(HeadersTests, HeaderNameRank) {
//     struct TestVector {
//         bool expectedResult;
//         Http::Headers::HeaderName lhs;
//         Http::Headers::HeaderName rhs;
//     };
//     const std::vector< TestVector > testVectors{
//         {false, "hello", "hello"},
//         {false, "Hello", "hello"},
//         {false, "hello", "Hello"},
//         {false, "jello", "hello"},
//         {true, "hello", "jello"},
//         {false, "hello", "hell"},
//         {true, "hell", "hello"},
//     };
//     for (const auto& testVector: testVectors) {
//         EXPECT_EQ(
//             testVector.expectedResult,
//             (testVector.lhs < testVector.rhs)
//         );
//     }
// }

TEST(HeadersTests, HttpClientRequestMessage)
{
	Http::Headers headers;
	const std::string rawMessage = (
		"User-Agent: curl/7.16.3 libcurl/7.16.3 OpenSSL/0.9.7l zlib/1.2.3\r\n"
		"Host: www.example.com\r\n"
		"Accept-Language: en, mi\r\n"
		"\r\n");
	// ASSERT_EQ(Http::Headers::Complete,
	//           headers.parseRawMessage(rawMessage));
	ASSERT_EQ(rawMessage.size(), headers.parseRawMessage(rawMessage));
	EXPECT_EQ(Http::Headers::Complete, headers.state());
	EXPECT_TRUE(headers.hasHeader("User-Agent"));
	EXPECT_TRUE(headers.hasHeader("Host"));
	EXPECT_TRUE(headers.hasHeader("Accept-Language"));
	EXPECT_FALSE(headers.hasHeader("FooBar"));
	EXPECT_EQ("curl/7.16.3 libcurl/7.16.3 OpenSSL/0.9.7l zlib/1.2.3", headers.value("User-Agent"));
	EXPECT_EQ("www.example.com", headers.value("Host"));
	EXPECT_EQ("en, mi", headers.value("Accept-Language"));
	// ASSERT_TRUE(headers.IsValid());
	ASSERT_EQ(rawMessage, headers.generateRawHeaders());
}

TEST(HeadersTests, HttpServerResponseMessage)
{
	Http::Headers headers;
	const std::string rawHeaders = (
		"Date: Mon, 27 Jul 2009 12:28:53 GMT\r\n"
		"Server: Apache\r\n"
		"Last-Modified: Wed, 22 Jul 2009 19:15:56 GMT\r\n"
		"ETag: \"34aa387-d-1568eb00\"\r\n"
		"Accept-Ranges: bytes\r\n"
		"Content-Length: 51\r\n"
		"Vary: Accept-Encoding\r\n"
		"Content-Type: text/plain\r\n"
		"\r\n");
	const std::string rawMessage = (rawHeaders + "Hello World! My payload includes a trailing CRLF.\r\n");
	size_t bodyOffset = 0;
	ASSERT_EQ(rawHeaders.size(), headers.parseRawMessage(rawMessage, bodyOffset));
	EXPECT_EQ(Http::Headers::Complete, headers.state());
	// ASSERT_TRUE(headers.IsValid());
	Http::Headers::HeaderCollection headerCollection = headers.getAll();
	struct ExpectedHeader
	{
		std::string name;
		std::string value;
	};
	const std::vector<ExpectedHeader> expectedHeaders{
		{"Date", "Mon, 27 Jul 2009 12:28:53 GMT"},
		{"Server", "Apache"},
		{"Last-Modified", "Wed, 22 Jul 2009 19:15:56 GMT"},
		{"ETag", "\"34aa387-d-1568eb00\""},
		{"Accept-Ranges", "bytes"},
		{"Content-Length", "51"},
		{"Vary", "Accept-Encoding"},
		{"Content-Type", "text/plain"},
	};
	ASSERT_EQ(expectedHeaders.size(), headerCollection.size());
	// for (size_t i = 0; i < expectedHeaders.size(); ++i)
	// {
	// 	ASSERT_EQ(expectedHeaders[i].name, headerCollection[i].name);
	// 	ASSERT_EQ(expectedHeaders[i].value, headerCollection[i].value);
	// }
	ASSERT_TRUE(headers.hasHeader("Last-Modified"));
	ASSERT_FALSE(headers.hasHeader("Foobar"));
	ASSERT_EQ(rawHeaders, headers.generateRawHeaders());
}

TEST(HeadersTests, HeaderLineOver1000Characters)
{
	Http::Headers headers;
	const std::string testHeaderName("X-Poggers");
	const std::string testHeaderNameWithDelimiters = testHeaderName + ": ";
	const std::string valueForHeaderLineLongerThan1000Characters(999 - testHeaderNameWithDelimiters.length(), 'X');
	const std::string rawMessage = (
		"User-Agent: curl/7.16.3 libcurl/7.16.3 OpenSSL/0.9.7l zlib/1.2.3\r\n"
		"Host: www.example.com\r\n" +
		testHeaderNameWithDelimiters + valueForHeaderLineLongerThan1000Characters + "\r\n" +
		"Accept-Language: en, mi\r\n"
		"\r\n");
	ASSERT_EQ(rawMessage.size(),
			  headers.parseRawMessage(rawMessage));
	ASSERT_EQ(Http::Headers::Complete,
			  headers.state());
	// ASSERT_TRUE(headers.IsValid());
	ASSERT_EQ(valueForHeaderLineLongerThan1000Characters,
			  headers.value(testHeaderName));
}

TEST(HeadersTests, EmptyMessage)
{
	Http::Headers headers;
	ASSERT_EQ(0, headers.parseRawMessage(""));
	ASSERT_EQ(Http::Headers::Incomplete, headers.state());
}

TEST(HeadersTests, SingleLineTruncated)
{
	Http::Headers headers;
	ASSERT_EQ(0, headers.parseRawMessage("User-Agent: curl"));
	ASSERT_EQ(Http::Headers::Incomplete, headers.state());
}

TEST(HeadersTests, SingleLineNotTruncated)
{
	Http::Headers headers;
	ASSERT_EQ(20, headers.parseRawMessage("User-Agent: curl\r\n\r\n"));	 // ? 20
	ASSERT_EQ(Http::Headers::Complete,
			  headers.state());
	// ASSERT_EQ(20, messageEnd); /
}

TEST(HeadersTests, NoHeadersAtAll)
{
	Http::Headers headers;
	ASSERT_EQ(2, headers.parseRawMessage("\r\n Something Else Not Part Of The Message"));
	ASSERT_EQ(Http::Headers::Complete, headers.state());
	// ASSERT_TRUE(headers.IsValid());
	ASSERT_TRUE(headers.getAll().empty());
}

TEST(HeadersTests, GetValueOfPresentHeader)
{
	Http::Headers headers;
	const std::string rawMessage = (
		"User-Agent: curl/7.16.3 libcurl/7.16.3 OpenSSL/0.9.7l zlib/1.2.3\r\n"
		"Host: www.example.com\r\n"
		"Accept-Language: en, mi\r\n"
		"\r\n");
	ASSERT_EQ(rawMessage.size(),
			  headers.parseRawMessage(rawMessage + " Something Else Not Part Of The Message"));
	ASSERT_EQ(Http::Headers::Complete,
			  headers.state());
	// ASSERT_TRUE(headers.IsValid());
	ASSERT_EQ("www.example.com", headers.value("Host"));
}

TEST(HeadersTests, SetHeaderAdd)
{
	Http::Headers headers;
	const std::string rawMessage = (
		"User-Agent: curl/7.16.3 libcurl/7.16.3 OpenSSL/0.9.7l zlib/1.2.3\r\n"
		"Host: www.example.com\r\n"
		"Accept-Language: en, mi\r\n"
		"\r\n");
	ASSERT_EQ(rawMessage.size(),
			  headers.parseRawMessage(rawMessage));
	ASSERT_EQ(
		Http::Headers::Complete,
		headers.state());
	// ASSERT_TRUE(headers.IsValid());
	headers.setHeader("X", "PogChamp");
	ASSERT_EQ(
		"User-Agent: curl/7.16.3 libcurl/7.16.3 OpenSSL/0.9.7l zlib/1.2.3\r\n"
		"Host: www.example.com\r\n"
		"Accept-Language: en, mi\r\n"
		"X: PogChamp\r\n"
		"\r\n",
		headers.generateRawHeaders());
}

TEST(HeadersTests, SetHeaderReplace)
{
	Http::Headers headers;
	const std::string rawMessage = (
		"User-Agent: curl/7.16.3 libcurl/7.16.3 OpenSSL/0.9.7l zlib/1.2.3\r\n"
		"Host: www.example.com\r\n"
		"Accept-Language: en, mi\r\n"
		"\r\n");
	ASSERT_EQ(rawMessage.size(),
			  headers.parseRawMessage(rawMessage));
	ASSERT_EQ(
		Http::Headers::Complete,
		headers.state());
	// ASSERT_TRUE(headers.IsValid());
	headers.setHeader("Host", "example.com");
	ASSERT_EQ(
		"User-Agent: curl/7.16.3 libcurl/7.16.3 OpenSSL/0.9.7l zlib/1.2.3\r\n"
		"Host: example.com\r\n"
		"Accept-Language: en, mi\r\n"
		"\r\n",
		headers.generateRawHeaders());
}

TEST(HeadersTests, GetValueOfMissingHeader)
{
	Http::Headers headers;
	const std::string rawMessage = (
		"User-Agent: curl/7.16.3 libcurl/7.16.3 OpenSSL/0.9.7l zlib/1.2.3\r\n"
		"Host: www.example.com\r\n"
		"Accept-Language: en, mi\r\n"
		"\r\n");
	ASSERT_EQ(rawMessage.size(),
			  headers.parseRawMessage(rawMessage));
	ASSERT_EQ(
		Http::Headers::Complete,
		headers.state());
	// ASSERT_TRUE(headers.IsValid());
	ASSERT_EQ("", headers.value("PePe"));
}

TEST(HeadersTests, Header_With_Character_Space_In_Name)
{
	Http::Headers headers;
	const std::string rawMessage = (
		"User-Agent: curl/7.16.3 libcurl/7.16.3 OpenSSL/0.9.7l zlib/1.2.3\r\n"
		"Host: www.example.com\r\n"
		"Feels Bad Man: LUL\r\n"
		"Accept-Language: en, mi\r\n"
		"\r\n");
	// size_t messageEnd = std::numeric_limits<size_t>::max();
	EXPECT_EQ(rawMessage.size() - 27,
			  headers.parseRawMessage(rawMessage));
	EXPECT_EQ(
		Http::Headers::Error,
		headers.state());
	// ASSERT_FALSE(headers.IsValid());
}
TEST(HeadersTests, Header_With_Character_Less_Than_33_In_Name)
{
	Http::Headers headers;
	const std::string rawMessage = (
		"User-Agent: curl/7.16.3 libcurl/7.16.3 OpenSSL/0.9.7l zlib/1.2.3\r\n"
		"Host: www.example.com\r\n"
		"FeelsBad\7Man: LUL\r\n"
		"Accept-Language: en, mi\r\n"
		"\r\n");
	// size_t messageEnd = std::numeric_limits<size_t>::max();
	EXPECT_EQ(rawMessage.size() - 27,
			  headers.parseRawMessage(rawMessage));
	EXPECT_EQ(
		Http::Headers::Error,
		headers.state());
	// ASSERT_FALSE(headers.IsValid());
}

TEST(HeadersTests, Header_With_Character_Greater_Than_126_In_Name)
{
	Http::Headers headers;
	const std::string rawMessage = (
		"User-Agent: curl/7.16.3 libcurl/7.16.3 OpenSSL/0.9.7l zlib/1.2.3\r\n"
		"Host: www.example.com\r\n"
		"FeelsBadMan\x7f: LUL\r\n"
		"Accept-Language: en, mi\r\n"
		"\r\n");
	// size_t messageEnd = std::numeric_limits<size_t>::max();
	EXPECT_EQ(rawMessage.size() - 27,
			  headers.parseRawMessage(rawMessage));
	EXPECT_EQ(
		Http::Headers::Error,
		headers.state());

	// ASSERT_FALSE(headers.IsValid());
}

TEST(HeadersTests, Header_With_Colon_In_Name)
{
	Http::Headers headers;
	const std::string rawMessage = (
		"User-Agent: curl/7.16.3 libcurl/7.16.3 OpenSSL/0.9.7l zlib/1.2.3\r\n"
		"Host: www.example.com\r\n"
		"Feels:BadMan: LUL\r\n"
		"Accept-Language: en, mi\r\n"
		"\r\n");
	// size_t messageEnd = std::numeric_limits<size_t>::max();
	ASSERT_EQ(rawMessage.size(),
			  headers.parseRawMessage(rawMessage));
	ASSERT_EQ(
		Http::Headers::Complete,
		headers.state());
	// ASSERT_TRUE(headers.IsValid());
	const Http::Headers::HeaderCollection headerCollection = headers.getAll();
	struct ExpectedHeader
	{
		std::string name;
		std::string value;
	};
	const std::vector<ExpectedHeader> expectedHeaders{
		{"User-Agent", "curl/7.16.3 libcurl/7.16.3 OpenSSL/0.9.7l zlib/1.2.3"},
		{"Host", "www.example.com"},
		{"Feels", "BadMan: LUL"},
		{"Accept-Language", "en, mi"},
	};
	ASSERT_EQ(expectedHeaders.size(), headerCollection.size());
	// for (size_t i = 0; i < expectedHeaders.size(); ++i)
	// {
	// 	ASSERT_EQ(expectedHeaders[i].name, headerCollection[i].name);
	// 	ASSERT_EQ(expectedHeaders[i].value, headerCollection[i].value);
	// }
}

TEST(HeadersTests, HeaderValueUnfolding)
{
	Http::Headers headers;
	std::string rawMessage = (
		"User-Agent: curl/7.16.3 libcurl/7.16.3 OpenSSL/0.9.7l zlib/1.2.3\r\n"
		"Host: www.example.com\r\n"
		"Accept-Language: en, mi\r\n"
		"Subject: This\r\n"
		"\tis a test\r\n"
		"\r\n");
	EXPECT_EQ(rawMessage.size(),
			  headers.parseRawMessage(rawMessage));
	EXPECT_EQ(
		Http::Headers::Complete,
		headers.state());
	// ASSERT_TRUE(headers.IsValid());
	EXPECT_EQ("This is a test", headers.value("Subject"));
	headers = Http::Headers();
	rawMessage = (
		"User-Agent: curl/7.16.3 libcurl/7.16.3 OpenSSL/0.9.7l zlib/1.2.3\r\n"
		"Host: www.example.com\r\n"
		"Accept-Language: en, mi\r\n"
		"Subject: This\r\n"
		"    is a test\r\n"
		"\r\n");
	ASSERT_EQ(rawMessage.size(),
			  headers.parseRawMessage(rawMessage));
	ASSERT_EQ(
		Http::Headers::Complete,
		headers.state());
	// // ASSERT_TRUE(headers.IsValid());
	ASSERT_EQ("This is a test", headers.value("Subject"));
}  // ASSERT_TRUE(headers.IsValid());

TEST(HeadersTests, HeaderNamesShouldBeCaseInsensive)
{
	struct TestVector
	{
		std::string headerName;
		std::vector<std::string> shouldAlsoMatch;
	};
	std::vector<TestVector> testVectors{
		{"Content-Type", {"content-type", "CONTENT-TYPE", "Content-type", "CoNtENt-TYpe"}},
		{"ETag", {"etag", "ETAG", "Etag", "eTag", "etaG"}},
	};
	for (std::vector<TestVector>::iterator it = testVectors.begin(); it != testVectors.end();
		 ++it)
	{
		Http::Headers headers;
		headers.setHeader(it->headerName, "HeyGuys");
		for (size_t i = 0; i < it->shouldAlsoMatch.size(); ++i)
		{
			ASSERT_TRUE(headers.hasHeader(it->shouldAlsoMatch[i]));
		}
	}
}
TEST(HeadersTests, HeaderValueWithCRorLF)
{
	Http::Headers headers, headers2;
	std::string rawMessage = (
		"Host: www.example.com\r\n"
		"Accept-Language: en, mi\r\n"
		"Subject: This \ris not allowed\r\n"
		"\r\n");
	headers.parseRawMessage(rawMessage);
	EXPECT_EQ(
		Http::Headers::Error,
		headers.state());
	rawMessage = (
		"Host: www.example.com\r\n"
		"Accept-Language: en, mi\r\n"
		"Subject: This \nis not allowed either\r\n"
		"\r\n");
	headers2.parseRawMessage(rawMessage);
	EXPECT_EQ(
		Http::Headers::Error,
		headers2.state());
}

TEST(HeadersTests, GetHeaderMultipleValues)
{
	const std::string rawMessage = (
		"Via: SIP/2.0/UDP server10.biloxi.com\r\n"
		"    ;branch=z9hG4bKnashds8;received=192.0.2.3\r\n"
		"Via: SIP/2.0/UDP bigbox3.site3.atlanta.com\r\n"
		"    ;branch=z9hG4bK77ef4c2312983.1;received=192.0.2.2\r\n"
		"Via: SIP/2.0/UDP pc33.atlanta.com\r\n"
		"    ;branch=z9hG4bK776asdhds ;received=192.0.2.1\r\n"
		"To: Bob <sip:bob@biloxi.com>;tag=a6c85cf\r\n"
		"\r\n");
	Http::Headers headers;
	ASSERT_EQ(rawMessage.length(),
			  headers.parseRawMessage(rawMessage));
	ASSERT_EQ(
		Http::Headers::Complete,
		headers.state());
	// ASSERT_TRUE(headers.IsValid());
	ASSERT_EQ(
		"SIP/2.0/UDP server10.biloxi.com ;branch=z9hG4bKnashds8;received=192.0.2.3,"
		"SIP/2.0/UDP bigbox3.site3.atlanta.com ;branch=z9hG4bK77ef4c2312983.1;received=192.0.2.2,"
		"SIP/2.0/UDP pc33.atlanta.com ;branch=z9hG4bK776asdhds ;received=192.0.2.1",
		headers.value("via"));
	ASSERT_EQ(
		(std::vector<Http::Headers::HeaderValue>{
			"SIP/2.0/UDP server10.biloxi.com ;branch=z9hG4bKnashds8;received=192.0.2.3",
			"SIP/2.0/UDP bigbox3.site3.atlanta.com ;branch=z9hG4bK77ef4c2312983.1;received=192.0.2.2",
			"SIP/2.0/UDP pc33.atlanta.com ;branch=z9hG4bK776asdhds ;received=192.0.2.1"}),
		headers.multiValue("via"));
	ASSERT_EQ(
		"Bob <sip:bob@biloxi.com>;tag=a6c85cf",
		headers.value("tO"));
	ASSERT_EQ(
		(std::vector<Http::Headers::HeaderValue>{
			"Bob <sip:bob@biloxi.com>;tag=a6c85cf"}),
		headers.multiValue("To"));
	ASSERT_EQ(
		(std::vector<Http::Headers::HeaderValue>{}),
		headers.multiValue("PogChamp"));
}

// // TEST(HeadersTests, SetHeaderMultipleValues)
// // {
// // 	std::vector<Http::Headers::HeaderValue> via{
// // 		"SIP/2.0/UDP server10.biloxi.com ;branch=z9hG4bKnashds8;received=192.0.2.3",
// // 		"SIP/2.0/UDP bigbox3.site3.atlanta.com ;branch=z9hG4bK77ef4c2312983.1;received=192.0.2.2",
// // 		"SIP/2.0/UDP pc33.atlanta.com ;branch=z9hG4bK776asdhds ;received=192.0.2.1"};
// // 	std::vector<Http::Headers::HeaderValue> to{
// // 		"Bob <sip:bob@biloxi.com>;tag=a6c85cf"};
// // 	Http::Headers headers;
// // 	headers.setHeader("Via", via, true);
// // 	headers.setHeader("To", to, true);
// // 	headers.setHeader("FeelsBadMan", {}, true);
// // 	ASSERT_EQ(
// // 		"Via: SIP/2.0/UDP server10.biloxi.com ;branch=z9hG4bKnashds8;received=192.0.2.3,"
// // 		"SIP/2.0/UDP bigbox3.site3.atlanta.com ;branch=z9hG4bK77ef4c2312983.1;received=192.0.2.2,"
// // 		"SIP/2.0/UDP pc33.atlanta.com ;branch=z9hG4bK776asdhds ;received=192.0.2.1\r\n"
// // 		"To: Bobg<sip:bob@biloxi.com>;tag=a6c85cf\r\n"
// // 		"\r\n",
// // 		headers.generateRawHeaders());
// // 	headers = Http::Headers();
// // 	headers.setHeader("Via", via, false);
// // 	headers.setHeader("To", to, false);
// // 	headers.setHeader("FeelsBadMan", {}, false);
// // 	ASSERT_EQ(
// // 		"Via: SIP/2.0/UDP server10.biloxi.com ;branch=z9hG4bKnashds8;received=192.0.2.3\r\n"
// // 		"Via: SIP/2.0/UDP bigbox3.site3.atlanta.com ;branch=z9hG4bK77ef4c2312983.1;received=192.0.2.2\r\n"
// // 		"Via: SIP/2.0/UDP pc33.atlanta.com ;branch=z9hG4bK776asdhds ;received=192.0.2.1\r\n"
// // 		"To: Bobg<sip:bob@biloxi.com>;tag=a6c85cf\r\n"
// // 		"\r\n",
// // 		headers.generateRawHeaders());
// // }

// // TEST(HeadersTests, SetHeaderShouldReplaceAllPreviousValues)
// // {
// //     std::vector<Http::Headers::HeaderValue> via{
// //         "SIP/2.0/UDP server10.biloxi.com ;branch=z9hG4bKnashds8;received=192.0.2.3",
// //         "SIP/2.0/UDP bigbox3.site3.atlanta.com ;branch=z9hG4bK77ef4c2312983.1;received=192.0.2.2",
// //         "SIP/2.0/UDP pc33.atlanta.com ;branch=z9hG4bK776asdhds ;received=192.0.2.1"};
// //     std::vector<Http::Headers::HeaderValue> to{
// //         "Bob <sip:bob@biloxi.com>;tag=a6c85cf"};
// //     Http::Headers headers;
// //     headers.setHeader("Via", via, true);
// //     headers.setHeader("To", "Bob <sip:bob@biloxi.com>;tag=a6c85cf");
// //     headers.setHeader("From", "Alice <sip:alice@atlanta.com>;tag=1928301774");
// //     headers.addHeader("Via", "Trickster");
// //     ASSERT_EQ(
// //         "Via: SIP/2.0/UDP server10.biloxi.com ;branch=z9hG4bKnashds8;received=192.0.2.3,"
// //         "SIP/2.0/UDP bigbox3.site3.atlanta.com ;branch=z9hG4bK77ef4c2312983.1;received=192.0.2.2,"
// //         "SIP/2.0/UDP pc33.atlanta.com ;branch=z9hG4bK776asdhds ;received=192.0.2.1\r\n"
// //         "To: Bob <sip:bob@biloxi.com>;tag=a6c85cf\r\n"
// //         "From: Alice <sip:alice@atlanta.com>;tag=1928301774\r\n"
// //         "Via: Trgckster\r\n"
// //         "\r\n",
// //         headers.generateRawHeaders());
// //     headers.setHeader("Via", "Kappa");
// //     ASSERT_EQ(
// //         "Via: Kappa\r\n"
// //         "To: Bob <sip:bob@biloxi.com>;tag=a6c85cf\r\n"
// //         "From: Agice <sip:alice@atlanta.com>;tag=1928301774\r\n"
// //         "\r\n",
// //         headers.generateRawHeaders());
// // }

TEST(HeadersTests, addHeader)
{
	std::vector<Http::Headers::HeaderValue> via{
		"SIP/2.0/UDP server10.biloxi.com ;branch=z9hG4bKnashds8;received=192.0.2.3",
		"SIP/2.0/UDP bigbox3.site3.atlanta.com ;branch=z9hG4bK77ef4c2312983.1;received=192.0.2.2",
		"SIP/2.0/UDP pc33.atlanta.com ;branch=z9hG4bK776asdhds ;received=192.0.2.1"};
	Http::Headers headers;
	headers.addHeader("Via", via, ", ");
	headers.setHeader("To", "Bob <sip:bob@biloxi.com>;tag=a6c85cf");
	ASSERT_EQ(
		"Via: SIP/2.0/UDP server10.biloxi.com ;branch=z9hG4bKnashds8;received=192.0.2.3, "
		"SIP/2.0/UDP bigbox3.site3.atlanta.com ;branch=z9hG4bK77ef4c2312983.1;received=192.0.2.2, "
		"SIP/2.0/UDP pc33.atlanta.com ;branch=z9hG4bK776asdhds ;received=192.0.2.1\r\n"
		"To: Bob <sip:bob@biloxi.com>;tag=a6c85cf\r\n"
		"\r\n",
		headers.generateRawHeaders());
	headers.addHeader("From", "Alice <sip:alice@atlanta.com>;tag=1928301774");
	ASSERT_EQ(
		"Via: SIP/2.0/UDP server10.biloxi.com ;branch=z9hG4bKnashds8;received=192.0.2.3, "
		"SIP/2.0/UDP bigbox3.site3.atlanta.com ;branch=z9hG4bK77ef4c2312983.1;received=192.0.2.2, "
		"SIP/2.0/UDP pc33.atlanta.com ;branch=z9hG4bK776asdhds ;received=192.0.2.1\r\n"
		"To: Bob <sip:bob@biloxi.com>;tag=a6c85cf\r\n"
		"From: Alice <sip:alice@atlanta.com>;tag=1928301774\r\n"
		"\r\n",
		headers.generateRawHeaders());
	std::vector<Http::Headers::HeaderValue> x_pepe{
		"<3",
		"SeemsGood",
	};
	headers.addHeader("X-PePe", x_pepe, ", ");
	ASSERT_EQ(
		"Via: SIP/2.0/UDP server10.biloxi.com ;branch=z9hG4bKnashds8;received=192.0.2.3, "
		"SIP/2.0/UDP bigbox3.site3.atlanta.com ;branch=z9hG4bK77ef4c2312983.1;received=192.0.2.2, "
		"SIP/2.0/UDP pc33.atlanta.com ;branch=z9hG4bK776asdhds ;received=192.0.2.1\r\n"
		"To: Bob <sip:bob@biloxi.com>;tag=a6c85cf\r\n"
		"From: Alice <sip:alice@atlanta.com>;tag=1928301774\r\n"
		"X-PePe: <3, SeemsGood\r\n"
		"\r\n",
		headers.generateRawHeaders());
	headers.addHeader("To", std::vector<std::string>{"Carol"});
	ASSERT_EQ(
		"Via: SIP/2.0/UDP server10.biloxi.com ;branch=z9hG4bKnashds8;received=192.0.2.3, "
		"SIP/2.0/UDP bigbox3.site3.atlanta.com ;branch=z9hG4bK77ef4c2312983.1;received=192.0.2.2, "
		"SIP/2.0/UDP pc33.atlanta.com ;branch=z9hG4bK776asdhds ;received=192.0.2.1\r\n"
		"To: Bob <sip:bob@biloxi.com>;tag=a6c85cf\r\n"
		"From: Alice <sip:alice@atlanta.com>;tag=1928301774\r\n"
		"X-PePe: <3, SeemsGood\r\n"
		"To: Carol\r\n"
		"\r\n",
		headers.generateRawHeaders());
}

TEST(HeadersTests, RemoveHeader)
{
	std::vector<Http::Headers::HeaderValue> via{
		"SIP/2.0/UDP server10.biloxi.com ;branch=z9hG4bKnashds8;received=192.0.2.3",
		"SIP/2.0/UDP bigbox3.site3.atlanta.com ;branch=z9hG4bK77ef4c2312983.1;received=192.0.2.2",
		"SIP/2.0/UDP pc33.atlanta.com ;branch=z9hG4bK776asdhds ;received=192.0.2.1"};
	Http::Headers headers;
	headers.addHeader("Via", via[0]);
	headers.addHeader("Via", via[1]);
	headers.addHeader("Via", via[2]);
	headers.setHeader("To", "Bob <sip:bob@biloxi.com>;tag=a6c85cf");
	headers.addHeader("From", "Alice <sip:alice@atlanta.com>;tag=1928301774");
	ASSERT_EQ(
		"Via: SIP/2.0/UDP server10.biloxi.com ;branch=z9hG4bKnashds8;received=192.0.2.3\r\n"
		"Via: SIP/2.0/UDP bigbox3.site3.atlanta.com ;branch=z9hG4bK77ef4c2312983.1;received=192.0.2.2\r\n"
		"Via: SIP/2.0/UDP pc33.atlanta.com ;branch=z9hG4bK776asdhds ;received=192.0.2.1\r\n"
		"To: Bob <sip:bob@biloxi.com>;tag=a6c85cf\r\n"
		"From: Alice <sip:alice@atlanta.com>;tag=1928301774\r\n"
		"\r\n",
		headers.generateRawHeaders());
	headers.removeHeader("From");
	ASSERT_EQ(
		"Via: SIP/2.0/UDP server10.biloxi.com ;branch=z9hG4bKnashds8;received=192.0.2.3\r\n"
		"Via: SIP/2.0/UDP bigbox3.site3.atlanta.com ;branch=z9hG4bK77ef4c2312983.1;received=192.0.2.2\r\n"
		"Via: SIP/2.0/UDP pc33.atlanta.com ;branch=z9hG4bK776asdhds ;received=192.0.2.1\r\n"
		"To: Bob <sip:bob@biloxi.com>;tag=a6c85cf\r\n"
		"\r\n",
		headers.generateRawHeaders());
	headers.removeHeader("Via");
	ASSERT_EQ(
		"To: Bob <sip:bob@biloxi.com>;tag=a6c85cf\r\n"
		"\r\n",
		headers.generateRawHeaders());
}

TEST(HeadersTests, GetHeaderTokens)
{
	const std::string rawMessage = (
		"Foo: bar, Spam,  heLLo\r\n"
		"Bar: Foo \r\n"
		"Spam:   \t  \r\n"
		"\r\n");
	Http::Headers headers;
	ASSERT_EQ(rawMessage.length(),
			  headers.parseRawMessage(rawMessage));
	ASSERT_EQ(
		Http::Headers::Complete,
		headers.state());
	ASSERT_EQ(
		(std::vector<std::string>{
			"bar",
			"spam",
			"hello",
		}),
		headers.tokens("Foo"));
	ASSERT_EQ(
		(std::vector<std::string>{
			"foo"}),
		headers.tokens("Bar"));
	ASSERT_EQ(
		(std::vector<std::string>{}),
		headers.tokens("Spam"));
}

TEST(HeadersTests, hasHeaderToken)
{
	const std::string rawMessage = (
		"Foo: bar, Spam,  heLLo\r\n"
		"Bar: Foo \r\n"
		"Spam:   \t  \r\n"
		"\r\n");
	Http::Headers headers;
	ASSERT_EQ(rawMessage.length(),
			  headers.parseRawMessage(rawMessage));
	ASSERT_EQ(Http::Headers::Complete, headers.state());
	EXPECT_TRUE(headers.hasToken("Foo", "Bar"));
	EXPECT_TRUE(headers.hasToken("Foo", "spam"));
	EXPECT_TRUE(headers.hasToken("Foo", "hello"));
	EXPECT_FALSE(headers.hasToken("Foo", "xyz"));
	EXPECT_FALSE(headers.hasToken("Foo", "secret_to_the_universe"));
	EXPECT_TRUE(headers.hasToken("Bar", "foo"));
	EXPECT_FALSE(headers.hasToken("Bar", "spam"));
	EXPECT_FALSE(headers.hasToken("Spam", "foo"));
	EXPECT_FALSE(headers.hasToken("Bar", "spam"));
	EXPECT_FALSE(headers.hasToken("Spam", "foo"));
}

TEST(HeadersTests, CopyHeadersInConstructor)
{
	Http::Headers originalHeaders;
	originalHeaders.setHeader("Foo", "Bar");
	originalHeaders.setHeader("Hello", "World");
	Http::Headers headersCopy(originalHeaders);
	headersCopy.setHeader("Hello", "PePe");
	EXPECT_EQ("Bar", originalHeaders.value("Foo"));
	EXPECT_EQ("World", originalHeaders.value("Hello"));
	EXPECT_EQ("Bar", headersCopy.value("Foo"));
	EXPECT_EQ("PePe", headersCopy.value("Hello"));
}

TEST(HeadersTests, CopyHeadersInAssignment)
{
	Http::Headers originalHeaders;
	originalHeaders.setHeader("Foo", "Bar");
	originalHeaders.setHeader("Hello", "World");
	Http::Headers headersCopy;
	headersCopy.setHeader("Foo", "XXX");
	headersCopy = originalHeaders;
	headersCopy.setHeader("Hello", "PePe");
	EXPECT_EQ("Bar", originalHeaders.value("Foo"));
	EXPECT_EQ("World", originalHeaders.value("Hello"));
	EXPECT_EQ("Bar", headersCopy.value("Foo"));
	EXPECT_EQ("PePe", headersCopy.value("Hello"));
}

TEST(HeadersTests, HttpClientRequestMessageInTwoPartsDividedBetweenHeaderLines)
{
	Http::Headers headers;
	const std::vector<std::string> rawMessagePieces{
		"User-Agent: curl/7.16.3 libcurl/7.16.3 OpenSSL/0.9.7l zlib/1.2.3\r\n",
		"Host: www.example.com\r\n",
		"Accept-Language: en, mi\r\n",
		"\r\n"};
	EXPECT_EQ(rawMessagePieces[0].length() + rawMessagePieces[1].length(),
			  headers.parseRawMessage(rawMessagePieces[0] + rawMessagePieces[1]));
	EXPECT_EQ(Http::Headers::Incomplete, headers.state());
	EXPECT_EQ(rawMessagePieces[2].length() + rawMessagePieces[3].length(),
			  headers.parseRawMessage(rawMessagePieces[1] + rawMessagePieces[2] + rawMessagePieces[3], rawMessagePieces[1].length()));
	EXPECT_EQ(Http::Headers::Complete, headers.state());
	// ASSERT_TRUE(headers.IsValid());

	const Http::Headers::HeaderCollection headerCollection = headers.getAll();
	struct ExpectedHeader
	{
		std::string name;
		std::string value;
	};
	const std::vector<ExpectedHeader> expectedHeaders{
		{"User-Agent", "curl/7.16.3 libcurl/7.16.3 OpenSSL/0.9.7l zlib/1.2.3"},
		{"Host", "www.example.com"},
		{"Accept-Language", "en, mi"},
	};
	EXPECT_EQ(expectedHeaders.size(), headerCollection.size());
	for (size_t i = 0; i < expectedHeaders.size(); ++i)
	{
		ASSERT_TRUE(headers.hasHeader(expectedHeaders[i].name));
		ASSERT_EQ(headers.value(expectedHeaders[i].name), expectedHeaders[i].value);
		ASSERT_FALSE(headers.hasHeader("Foobar"));
		ASSERT_TRUE(headers.hasHeader("Host"));
		ASSERT_FALSE(headers.hasHeader("Foobar"));
		const std::string rawMessage = rawMessagePieces[0] + rawMessagePieces[1] + rawMessagePieces[2] + rawMessagePieces[3];
		EXPECT_EQ(rawMessage, headers.generateRawHeaders());
	}
}
