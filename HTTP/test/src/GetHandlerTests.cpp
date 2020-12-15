#include <gtest/gtest.h>

#include <HTTP/GetHandler.hpp>
using namespace Http;
struct GetHandlerTests : public ::testing::Test
{
	std::vector<std::string> matches;
	void SetUp()
	{
		matches.push_back(".html");
		matches.push_back(".html.en");
		matches.push_back(".html.fr.utf-8");
		matches.push_back(".html.fr");
		matches.push_back(".se.html");
		matches.push_back(".utf-8.html");
	}
};

TEST_F(GetHandlerTests, getQuality)
{
	EXPECT_EQ(0, getQuality("en; q=0"));
	EXPECT_EQ(1000, getQuality("en"));
	EXPECT_EQ(1000, getQuality("en; "));
	EXPECT_EQ(1000, getQuality("en; h=1"));
	EXPECT_EQ(1000, getQuality("en; q=1"));
	EXPECT_EQ(1000, getQuality("en; q=1.00"));
	EXPECT_EQ(1000, getQuality("en; q=1"));
	EXPECT_EQ(100, getQuality("en; q=0.1"));
	EXPECT_EQ(100, getQuality("en; q=.1"));
}
TEST_F(GetHandlerTests, find_matches)
{
	std::vector<std::string> LanguageTokens;
	LanguageTokens.push_back("en; q=0.2");
	LanguageTokens.push_back("fr; q=0.9");
	LanguageTokens.push_back("se; q=0.5");
	std::string matched = matchByLanguage(LanguageTokens, matches);
	EXPECT_EQ("fr", matched);
	EXPECT_EQ(2, matches.size());
	EXPECT_EQ(".html.fr.utf-8", matches[0]);
	EXPECT_EQ(".html.fr", matches[1]);
}

TEST_F(GetHandlerTests, matchByLanguageNoToken)
{
	std::vector<std::string> LanguageTokens;
	LanguageTokens.push_back("en");
	LanguageTokens.push_back("fr; q=0.9");
	LanguageTokens.push_back("se; q=0.5");
	std::string matched = matchByLanguage(LanguageTokens, matches);
	EXPECT_EQ(matched, "en");
	EXPECT_EQ(matches.size(), 1);
}
TEST_F(GetHandlerTests, matchByLanguageNoMatch)
{
	std::vector<std::string> LanguageTokens;
	LanguageTokens.push_back("en-gb");
	std::string matched = matchByLanguage(LanguageTokens, matches);
	EXPECT_EQ(matched, "");
	EXPECT_EQ(matches.size(), 6);
}
TEST_F(GetHandlerTests, matchByLanguageLast)
{
	std::vector<std::string> LanguageTokens;
	LanguageTokens.push_back("en");
	LanguageTokens.push_back("fr; q=0.9");
	LanguageTokens.push_back("se; q=0.5");
	std::string matched = matchByLanguage(LanguageTokens, matches);
	EXPECT_EQ(matched, "en");
	EXPECT_EQ(matches.size(), 1);
}

TEST_F(GetHandlerTests, matchByLanguageFirst)
{
	std::vector<std::string> LanguageTokens;
	LanguageTokens.push_back("en; q=0.2");
	LanguageTokens.push_back("fr; q=0.9");
	LanguageTokens.push_back("se;");
	std::string matched = matchByLanguage(LanguageTokens, matches);
	EXPECT_EQ(matched, "se");
	EXPECT_EQ(matches.size(), 1);
}
TEST_F(GetHandlerTests, matchByCharset)
{
	std::vector<std::string> CharsetTokens;
	CharsetTokens.push_back("utf-8; q=0.2");
	CharsetTokens.push_back("gz; q=0.9");
	std::string matched = matchByCharset(CharsetTokens, matches);
	EXPECT_EQ(matched, "utf-8");
	EXPECT_EQ(matches.size(), 2);
}
