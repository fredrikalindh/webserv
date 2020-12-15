#include <gtest/gtest.h>

#include <Config/MimeTypes.hpp>

TEST(MimetypesTests, oktest)
{
	EXPECT_TRUE(true);
}

TEST(MimetypesTests, existing)
{
	EXPECT_EQ(Mime::type(".html"), "text/html");
	EXPECT_EQ(Mime::type(".htm"), "text/html");
	EXPECT_EQ(Mime::type(".css"), "text/css");
	EXPECT_EQ(Mime::type(".jpg"), "image/jpeg");
	EXPECT_EQ(Mime::type(".mp4"), "video/mp4");
	EXPECT_EQ(Mime::type(".xhtml"), "application/xhtml+xml");
	EXPECT_EQ(Mime::type(".txt"), "text/plain");
	EXPECT_EQ(Mime::type(".odg"), "application/vnd.oasis.opendocument.graphics");
	EXPECT_EQ(Mime::type(".pptx"), "application/vnd.openxmlformats-officedocument.presentationml.presentation");
}
TEST(MimetypesTests, with_path)
{
	EXPECT_EQ(Mime::type("myfolder/myfile.html"), "text/html");
	EXPECT_EQ(Mime::type("myfolder/myfile.htm"), "text/html");
	EXPECT_EQ(Mime::type("myfolder/myfile.css"), "text/css");
	EXPECT_EQ(Mime::type("myfolder/myfile.jpg"), "image/jpeg");
	EXPECT_EQ(Mime::type("myfolder/myfile.mp4"), "video/mp4");
	EXPECT_EQ(Mime::type("myfolder/myfile.xhtml"), "application/xhtml+xml");
	EXPECT_EQ(Mime::type("myfolder/myfile.txt"), "text/plain");
	EXPECT_EQ(Mime::type("myfolder/myfile.odg"), "application/vnd.oasis.opendocument.graphics");
	EXPECT_EQ(Mime::type("myfolder/myfile.pptx"), "application/vnd.openxmlformats-officedocument.presentationml.presentation");
}
TEST(MimetypesTests, CaseInsensitive)
{
	EXPECT_EQ(Mime::type(".HTML"), "text/html");
	EXPECT_EQ(Mime::type(".HTM"), "text/html");
	EXPECT_EQ(Mime::type(".CSS"), "text/css");
	EXPECT_EQ(Mime::type(".JPG"), "image/jpeg");
	EXPECT_EQ(Mime::type(".Mp4"), "video/mp4");
	EXPECT_EQ(Mime::type(".XHtml"), "application/xhtml+xml");
	EXPECT_EQ(Mime::type(".tXt"), "text/plain");
	EXPECT_EQ(Mime::type(".odG"), "application/vnd.oasis.opendocument.graphics");
	EXPECT_EQ(Mime::type(".ppTx"), "application/vnd.openxmlformats-officedocument.presentationml.presentation");
}
TEST(MimetypesTests, inMiddleofFileName)
{
	EXPECT_EQ(Mime::type("myfolder/myfile.html.en"), "text/html");
	EXPECT_EQ(Mime::type("myfolder/myfile.htm.en"), "text/html");
	EXPECT_EQ(Mime::type("myfolder/myfile.css.en"), "text/css");
	EXPECT_EQ(Mime::type("myfolder/myfile.jpg.en"), "image/jpeg");
	EXPECT_EQ(Mime::type("myfolder/myfile.mp4.en"), "video/mp4");
	EXPECT_EQ(Mime::type("myfolder/myfile.xhtml.en"), "application/xhtml+xml");
	EXPECT_EQ(Mime::type("myfolder/myfile.txt.en"), "text/plain");
	EXPECT_EQ(Mime::type("myfolder/myfile.odg.en"), "application/vnd.oasis.opendocument.graphics");
	EXPECT_EQ(Mime::type("myfolder/myfile.pptx.en"), "application/vnd.openxmlformats-officedocument.presentationml.presentation");
}
TEST(MimetypesTests, inMiddleOfPath)
{
	EXPECT_EQ(Mime::type("myfolder/myfile.html.en/bad"), "application/octet-stream");
}
TEST(MimetypesTests, no_dot)  // ? SHOULD WORK OR NOT ?
{
	EXPECT_EQ(Mime::type("html"), "text/html");
	EXPECT_EQ(Mime::type("htm"), "text/html");
	EXPECT_EQ(Mime::type("css"), "text/css");
	EXPECT_EQ(Mime::type("jpg"), "image/jpeg");
	EXPECT_EQ(Mime::type("mp4"), "video/mp4");
	EXPECT_EQ(Mime::type("xhtml"), "application/xhtml+xml");
	EXPECT_EQ(Mime::type("txt"), "text/plain");
	EXPECT_EQ(Mime::type("odg"), "application/vnd.oasis.opendocument.graphics");
	EXPECT_EQ(Mime::type("pptx"), "application/vnd.openxmlformats-officedocument.presentationml.presentation");
}
// TEST(MimetypesTests, no_dot_with_path)
// {
// 	EXPECT_EQ(Mime::type("myfolder/html"), "text/html");
// 	EXPECT_EQ(Mime::type("myfolder/htm"), "text/html");
// 	EXPECT_EQ(Mime::type("myfolder/css"), "text/css");
// 	EXPECT_EQ(Mime::type("myfolder/jpg"), "image/jpeg");
// 	EXPECT_EQ(Mime::type("myfolder/mp4"), "video/mp4");
// 	EXPECT_EQ(Mime::type("myfolder/xhtml"), "application/xhtml+xml");
// 	EXPECT_EQ(Mime::type("myfolder/txt"), "text/plain");
// 	EXPECT_EQ(Mime::type("myfolder/odg"), "application/vnd.oasis.opendocument.graphics");
// 	EXPECT_EQ(Mime::type("myfolder/myfile.pptx"), "application/vnd.openxmlformats-officedocument.presentationml.presentation");
// }
TEST(MimetypesTests, non_existing)
{
	EXPECT_EQ(Mime::type(".xxx"), "application/octet-stream");
	EXPECT_EQ(Mime::type("xxx"), "application/octet-stream");
	EXPECT_EQ(Mime::type(""), "application/octet-stream");
	// EXPECT_EQ(Mime::type(".xxx"), "application/octet-stream");
	// EXPECT_EQ(Mime::type("xxx"), "application/octet-stream");
	// EXPECT_EQ(Mime::type(""), "application/octet-stream");
}
