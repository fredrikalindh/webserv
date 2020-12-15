#include <gtest/gtest.h>

#include <HTTP/Request.hpp>

using namespace Http;
TEST(RequestTest, startLine)
{
	std::string rawMessage("GET /search.html?name=fredrika&age=21#button HTTP/1.1");
	Request request;
	EXPECT_EQ(0, request.parse_first(rawMessage));
	EXPECT_EQ("GET", request.method);
	EXPECT_EQ("/search.html", request.path);
	EXPECT_EQ("name=fredrika&age=21", request.query);
	EXPECT_EQ("button", request.fragment);
}
TEST(RequestTest, badMethod)
{
	std::string rawMessage("HELLO /search.html?name=fredrika&age=21#button HTTP/1.1");
	Request request;
	EXPECT_EQ(501, request.parse_first(rawMessage));
}
TEST(RequestTest, badPath)
{
	std::string rawMessage("GET search.html?name=fredrika&age=21#button HTTP/1.1");
	Request request;
	EXPECT_EQ(400, request.parse_first(rawMessage));
}
TEST(RequestTest, badVersion)
{
	std::string rawMessage("GET search.html?name=fredrika&age=21#button HTTP/2.1");
	Request request;
	EXPECT_EQ(400, request.parse_first(rawMessage));
}
TEST(RequestTest, fragmentBeforeQuery)
{
	std::string rawMessage("GET /search.html#button?name=fredrika&age=21 HTTP/1.1");
	Request request;
	EXPECT_EQ(0, request.parse_first(rawMessage));
	EXPECT_EQ("GET", request.method);
	EXPECT_EQ("/search.html", request.path);
	EXPECT_EQ("", request.query);
	EXPECT_EQ("button?name=fredrika&age=21", request.fragment);
}
TEST(RequestTest, rootPath)
{
	std::string rawMessage("GET /?name=fredrika&age=21 HTTP/1.1");
	Request request;
	EXPECT_EQ(0, request.parse_first(rawMessage));
	EXPECT_EQ("GET", request.method);
	EXPECT_EQ("/", request.path);
	EXPECT_EQ("name=fredrika&age=21", request.query);
	EXPECT_EQ("", request.fragment);
}
TEST(RequestTest, decodeUrl)
{
	std::string rawPath("/action+G%C3%BCnter%C3%962.php?text=Hello+G%C3%BCnter%C3%96");
	Request request;
	EXPECT_TRUE(request.set_path(rawPath));
	EXPECT_EQ("/action GünterÖ2.php", request.path);
	EXPECT_EQ("text=Hello+G%C3%BCnter%C3%96", request.query);
}
TEST(RequestTest, decodeUrlUtf8)
{
	std::string rawPath("/Jag+%E2%9D%A4%EF%B8%8F+dig%21%21.html?text=Jag+%E2%9D%A4%EF%B8%8F+dig%21%21");
	Request request;
	EXPECT_TRUE(request.set_path(rawPath));
	EXPECT_EQ("/Jag ❤️ dig!!.html", request.path);
	EXPECT_EQ("text=Jag+%E2%9D%A4%EF%B8%8F+dig%21%21", request.query);
}
