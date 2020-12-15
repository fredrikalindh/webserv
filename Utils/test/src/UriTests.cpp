#include <gtest/gtest.h>

#include <Utils/Uri.hpp>

// using namespace Utils;
TEST(UriTests, EmptyPath)
{
	Uri uri("");
	std::vector<std::string> path = uri.path();
	ASSERT_EQ(0, path.size());
	EXPECT_EQ("", uri.str());
}
TEST(UriTests, RootPath)
{
	Uri uri("/");
	std::vector<std::string> path = uri.path();
	ASSERT_EQ(0, path.size());
	EXPECT_EQ("/", uri.str());
}
TEST(UriTests, SimplePath)
{
	std::string raw("/data/www/images/dog.jpg");
	Uri uri(raw);
	std::vector<std::string> path = uri.path();
	ASSERT_EQ(4, path.size());
	std::string subpaths[] = {"data", "www", "images", "dog.jpg"};
	EXPECT_EQ(raw, uri.str());
	for (size_t i = 0; i < path.size(); ++i)
		EXPECT_EQ(subpaths[i], path[i]);
}
TEST(UriTests, SimplePath2)
{
	std::string raw("data/www/images/dog.jpg");
	Uri uri(raw);
	std::vector<std::string> path = uri.path();
	ASSERT_EQ(4, path.size());
	std::string subpaths[] = {"data", "www", "images", "dog.jpg"};
	EXPECT_EQ(raw, uri.str());
	for (size_t i = 0; i < path.size(); ++i)
		EXPECT_EQ(subpaths[i], path[i]);
}
TEST(UriTests, DecodePath)
{
	Uri uri = Uri::decode("data/www/images/dog%20cute.jpg/");

	std::vector<std::string> path = uri.path();
	ASSERT_EQ(4, path.size());
	std::string subpaths[] = {"data", "www", "images", "dog cute.jpg"};
	EXPECT_EQ("data/www/images/dog cute.jpg", uri.str());
	for (size_t i = 0; i < path.size(); ++i)
		EXPECT_EQ(subpaths[i], path[i]);
}
TEST(UriTests, ReplacePlus)
{
	Uri uri = Uri::decode("data/www/images/dog+cute.jpg/");
	std::vector<std::string> path = uri.path();
	ASSERT_EQ(4, path.size());
	std::string subpaths[] = {"data", "www", "images", "dog cute.jpg"};
	EXPECT_EQ("data/www/images/dog cute.jpg", uri.str());
	for (size_t i = 0; i < path.size(); ++i)
		EXPECT_EQ(subpaths[i], path[i]);
}
TEST(UriTests, OperatorPlus)
{
	Uri uri1("data/www/index.php");
	Uri uri2("test/again");
	Uri uri3 = uri1 + uri2;
	EXPECT_EQ(3, uri1.path().size());
	EXPECT_EQ(2, uri2.path().size());
	EXPECT_EQ(5, uri3.path().size());

	EXPECT_EQ("data/www/index.php/test/again", uri3.str());
	uri2 += uri1;
	EXPECT_EQ("test/again/data/www/index.php", uri2.str());
}
TEST(UriTests, GetExtensions)
{
	std::vector<std::string> ext = Uri::getExtensions("file.html.en-US.utf-8");
	ASSERT_EQ(3, ext.size());
	EXPECT_EQ("html", ext[0]);
	EXPECT_EQ("en-US", ext[1]);
	EXPECT_EQ("utf-8", ext[2]);
}
TEST(UriTests, FindExtension)
{
	Uri uri("data/www/index.php/test");
	EXPECT_EQ(2, uri.findExt("php"));
}
TEST(UriTests, NotFindExtension)
{
	Uri uri("data/www/index.php/test");
	EXPECT_EQ(-1, uri.findExt("html"));
}
TEST(UriTests, FindExtensions)
{
	Uri uri("data/www/index.html/test");
	std::vector<std::string> ext({"php", "html"});
	std::pair<size_t, std::string> found = uri.findExt(ext);
	EXPECT_EQ(2, found.first);
	EXPECT_EQ("html", found.second);
}
TEST(UriTests, NotFindExtensions)
{
	Uri uri("data/www/index.html/test");
	std::vector<std::string> ext({"php", "test"});
	std::pair<size_t, std::string> found = uri.findExt(ext);
	EXPECT_EQ(-1, found.first);
	EXPECT_EQ("", found.second);
}
