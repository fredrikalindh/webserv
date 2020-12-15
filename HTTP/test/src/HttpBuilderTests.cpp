#include <gtest/gtest.h>

#include <HTTP/HttpBuilder.hpp>

struct HttpBuilderTests : public ::testing::Test
{
	std::vector<std::string> matches;
	void SetUp()
	{
	}
	void TearDown()
	{
	}
};

TEST_F(HttpBuilderTests, ContentEncoded)
{
}
