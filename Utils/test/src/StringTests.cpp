#include <gtest/gtest.h>
#include <limits.h>

#include <Utils/String.hpp>

// using namespace Utils;
TEST(UriTests, toupper)
{
	std::string test("testING fso $35");
	EXPECT_EQ("TESTING FSO $35", Utils::String::toupper(test));
	std::string test2("");
	EXPECT_EQ("", Utils::String::toupper(test2));
}
TEST(UriTests, tolower)
{
	std::string test("testING fso $35");
	EXPECT_EQ("testing fso $35", Utils::String::tolower(test));
	std::string test2("");
	EXPECT_EQ("", Utils::String::tolower(test2));
}
TEST(UriTests, split)
{
	std::string test("abc def gh1235");
	std::vector<std::string> ret = Utils::String::split(test);
	const std::vector<std::string> expectedRet{"abc", "def", "gh1235"};
	EXPECT_EQ(expectedRet, ret);
}
TEST(UriTests, split2)
{
	std::string test("abc     def   gh12      35");
	std::vector<std::string> ret = Utils::String::split(test);
	const std::vector<std::string> expectedRet{"abc", "def", "gh12", "35"};
	EXPECT_EQ(expectedRet, ret);
}
TEST(UriTests, split_set_char)
{
	std::string test("ab1c def h1235");
	std::vector<std::string> ret = Utils::String::split(test, '1');
	const std::vector<std::string> expectedRet{"ab", "c def h", "235"};
	EXPECT_EQ(expectedRet, ret);
}

TEST(UriTests, to_string)
{
	EXPECT_EQ("1235", Utils::String::to_string(1235));
	EXPECT_EQ("-1235", Utils::String::to_string(-1235));
	EXPECT_EQ("2147483647", Utils::String::to_string(INT_MAX));
	EXPECT_EQ("-2147483648", Utils::String::to_string(-2147483648));
}
TEST(UriTests, to_int)
{
	EXPECT_EQ(1235, Utils::String::to_int("1235"));
	EXPECT_EQ(-1235, Utils::String::to_int("-1235"));
	EXPECT_EQ(2147483647, Utils::String::to_int("2147483647"));
	EXPECT_EQ(-2147483648, Utils::String::to_int("-2147483648"));
	EXPECT_EQ(1235, Utils::String::to_int("1235ABC"));
	EXPECT_EQ(-1235, Utils::String::to_int("-1235 ABC"));
	EXPECT_EQ(0, Utils::String::to_int("--1235 ABC"));
	EXPECT_EQ(2147483647, Utils::String::to_int("2147483647ABC"));
	// EXPECT_EQ(-2147483648, Utils::String::to_int("-2147483648ABC"));
	EXPECT_THROW(Utils::String::to_int("-2147483649"), std::overflow_error);
	EXPECT_THROW(Utils::String::to_int("2147483648"), std::overflow_error);
}
