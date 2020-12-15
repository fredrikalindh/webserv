#include <gtest/gtest.h>

#include <Config/Location.hpp>

using namespace Config;

struct LocationTests : public ::testing::Test
{
	class LocationMock : public Location
	{
	public:
		LocationMock() : Location() {}
		void location(Parser<Token>::iterator &it)
		{
			Location::location(it);
		}
		void set_autoindex(Parser<Token>::iterator &it)
		{
			Location::set_autoindex(it);
		}
		void set_root(Parser<Token>::iterator &it)
		{
			Location::set_root(it);
		}
		void set_index(Parser<Token>::iterator &it)
		{
			Location::set_index(it);
		}
		void set_limit_except(Parser<Token>::iterator &it)
		{
			Location::set_limit_except(it);
		}
		void set_upload(Parser<Token>::iterator &it)
		{
			Location::set_upload(it);
		}
		void set_cgi(Parser<Token>::iterator &it)
		{
			Location::set_cgi(it);
		}
		void set_auth_basic(Parser<Token>::iterator &it)
		{
			Location::set_auth_basic(it);
		}
		void set_client_max_body_size(Parser<Token>::iterator &it)
		{
			Location::set_client_max_body_size(it);
		}
		void set_directive(Parser<Token>::iterator &it)
		{
			Location::set_directive(it);
		}
	};
	std::vector<Token> tokens_;
	LocationMock location_;

	void addTokens(std::string *begin, std::string *end)
	{
		while (begin != end)
			tokens_.push_back(Token(*begin++, 0));
	}
	virtual void SetUp()
	{
	}
	virtual void TearDown()
	{
	}
};

TEST_F(LocationTests, location_main)
{
	tokens_.push_back(Token("/", 0));
	tokens_.push_back(Token("{", 0));
	tokens_.push_back(Token("}", 0));
	Parser<Token>::iterator it = tokens_.begin();
	location_.location(it);
	EXPECT_FALSE(location_.exactMatch("/"));
	EXPECT_EQ(location_.uri(), "/");
}
TEST_F(LocationTests, location_exactMatch)
{
	tokens_.push_back(Token("=", 0));
	tokens_.push_back(Token("/images", 0));
	tokens_.push_back(Token("{", 0));
	tokens_.push_back(Token("}", 0));
	Parser<Token>::iterator it = tokens_.begin();
	location_.location(it);
	EXPECT_TRUE(location_.exactMatch("/images"));
	EXPECT_EQ(location_.uri(), "/images");
}
TEST_F(LocationTests, location_match)
{
	tokens_.push_back(Token("/images", 0));
	tokens_.push_back(Token("{", 0));
	tokens_.push_back(Token("}", 0));
	Parser<Token>::iterator it = tokens_.begin();
	location_.location(it);
	EXPECT_EQ(7, location_.matches("/images"));
	EXPECT_EQ(location_.uri(), "/images");
}
TEST_F(LocationTests, location_regexMatch)
{
	tokens_.push_back(Token("~", 0));
	tokens_.push_back(Token("\\.(jpe?g|png|gif|ico)$", 0));
	tokens_.push_back(Token("{", 0));
	tokens_.push_back(Token("}", 0));
	Parser<Token>::iterator it = tokens_.begin();
	location_.location(it);
	EXPECT_TRUE(location_.regexMatch("/image.jpg"));
	EXPECT_TRUE(location_.regexMatch("/flower.jpg"));
	EXPECT_FALSE(location_.regexMatch("/FLOWER.JPG"));
	EXPECT_FALSE(location_.regexMatch("/imagejpg"));
	EXPECT_FALSE(location_.regexMatch("/"));
	EXPECT_FALSE(location_.regexMatch("/img.giff"));
}
TEST_F(LocationTests, location_regexCaseInsensitive)
{
	tokens_.push_back(Token("~*", 0));
	tokens_.push_back(Token("\\.(jpe?g|png|gif|ico)$", 0));
	tokens_.push_back(Token("{", 0));
	tokens_.push_back(Token("}", 0));
	Parser<Token>::iterator it = tokens_.begin();
	location_.location(it);
	EXPECT_TRUE(location_.regexMatch("/image.jpg"));
	EXPECT_TRUE(location_.regexMatch("/FLOWER.JPG"));
	EXPECT_FALSE(location_.regexMatch("/IMAGEJPG"));
	EXPECT_FALSE(location_.regexMatch("/"));
	EXPECT_FALSE(location_.regexMatch("/img.giff"));
}
TEST_F(LocationTests, location_regexMatch2)
{
	tokens_.push_back(Token("~", 0));
	tokens_.push_back(Token("(.*/myapp)/(.+\\.php)$", 0));
	tokens_.push_back(Token("{", 0));
	tokens_.push_back(Token("}", 0));
	Parser<Token>::iterator it = tokens_.begin();
	location_.location(it);
	EXPECT_TRUE(location_.regexMatch("/myapp/hello.php"));
	EXPECT_TRUE(location_.regexMatch("/root/myapp/test.php"));
	EXPECT_FALSE(location_.regexMatch("app/test.php"));
	EXPECT_FALSE(location_.regexMatch("/root/myapp/test.png"));
	EXPECT_FALSE(location_.regexMatch("/root/myapp/test.phpp"));
}
TEST_F(LocationTests, location_full)
{
	tokens_.push_back(Token("=", 0));
	tokens_.push_back(Token("/images", 0));
	tokens_.push_back(Token("{", 0));
	tokens_.push_back(Token("autoindex", 1));
	tokens_.push_back(Token("on", 1));
	tokens_.push_back(Token(";", 1));
	tokens_.push_back(Token("root", 2));
	tokens_.push_back(Token("root", 2));
	tokens_.push_back(Token(";", 2));
	tokens_.push_back(Token("index", 3));
	tokens_.push_back(Token("index.html", 3));
	tokens_.push_back(Token(";", 3));
	tokens_.push_back(Token("cgi", 4));
	tokens_.push_back(Token("php", 4));
	tokens_.push_back(Token("/php-cgi", 4));
	tokens_.push_back(Token(";", 4));
	tokens_.push_back(Token("limit_except", 5));
	tokens_.push_back(Token("GET", 5));
	tokens_.push_back(Token(";", 5));
	tokens_.push_back(Token("upload", 6));
	tokens_.push_back(Token("/upload", 6));
	tokens_.push_back(Token(";", 6));
	tokens_.push_back(Token("}", 7));

	Parser<Token>::iterator it = tokens_.begin();
	location_.location(it);
	EXPECT_TRUE(location_.exactMatch("/images"));
	EXPECT_EQ(location_.uri(), "/images");
	EXPECT_TRUE(location_.autoindex());
	EXPECT_EQ(location_.root(), "root");
	EXPECT_EQ(location_.index()[0], "index.html");
	EXPECT_TRUE(location_.hasCGI("php"));
	EXPECT_EQ(location_.CGIpath("php"), "/php-cgi");
	EXPECT_TRUE(location_.allow("GET"));
	EXPECT_FALSE(location_.allow("POST"));
	EXPECT_EQ(location_.upload(), "/upload");
}

TEST_F(LocationTests, location_badUri)
{
	tokens_.push_back(Token("=", 0));
	tokens_.push_back(Token("badUri", 0));
	tokens_.push_back(Token("{", 0));
	tokens_.push_back(Token("}", 0));
	Parser<Token>::iterator it = tokens_.begin();
	EXPECT_THROW(location_.location(it), std::exception);
}
TEST_F(LocationTests, location_badDir)
{
	tokens_.push_back(Token("=", 0));
	tokens_.push_back(Token("/", 0));
	tokens_.push_back(Token("{", 0));
	tokens_.push_back(Token("badDir", 0));
	tokens_.push_back(Token("}", 0));
	Parser<Token>::iterator it = tokens_.begin();
	EXPECT_THROW(location_.location(it), std::exception);
}

TEST_F(LocationTests, location_missingBracket)
{
	tokens_.push_back(Token("=", 0));
	tokens_.push_back(Token("/", 0));
	tokens_.push_back(Token("}", 0));
	Parser<Token>::iterator it = tokens_.begin();
	EXPECT_THROW(location_.location(it), std::exception);
}

TEST_F(LocationTests, location_missingUri)
{
	tokens_.push_back(Token("=", 0));
	tokens_.push_back(Token("{", 0));
	tokens_.push_back(Token("}", 0));
	Parser<Token>::iterator it = tokens_.begin();
	EXPECT_THROW(location_.location(it), std::exception);
}

TEST_F(LocationTests, autoindex_main)
{
	tokens_.push_back(Token("on", 0));
	tokens_.push_back(Token(";", 0));
	Parser<Token>::iterator it = tokens_.begin();
	location_.set_autoindex(it);
	EXPECT_TRUE(location_.autoindex());
}
TEST_F(LocationTests, autoindex_off)
{
	tokens_.push_back(Token("off", 0));
	tokens_.push_back(Token(";", 0));
	Parser<Token>::iterator it = tokens_.begin();
	location_.set_autoindex(it);
	EXPECT_FALSE(location_.autoindex());
}
TEST_F(LocationTests, autoindex_other)
{
	tokens_.push_back(Token("xxx", 0));
	tokens_.push_back(Token(";", 0));
	Parser<Token>::iterator it = tokens_.begin();
	location_.set_autoindex(it);
	EXPECT_FALSE(location_.autoindex());
}
TEST_F(LocationTests, autoindex_doubleval)
{
	tokens_.push_back(Token("1", 0));
	tokens_.push_back(Token("2", 0));
	tokens_.push_back(Token(";", 0));
	Parser<Token>::iterator it = tokens_.begin();
	EXPECT_THROW(location_.set_autoindex(it), std::exception);
}

TEST_F(LocationTests, autoindex_noval)
{
	tokens_.push_back(Token(";", 0));
	Parser<Token>::iterator it = tokens_.begin();
	location_.set_autoindex(it);
	EXPECT_FALSE(location_.autoindex());
}

TEST_F(LocationTests, root_main)
{
	tokens_.push_back(Token("/root", 0));
	tokens_.push_back(Token(";", 0));
	Parser<Token>::iterator it = tokens_.begin();
	location_.set_root(it);
	EXPECT_EQ(location_.root(), "/root");
}
TEST_F(LocationTests, root_noSlash)
{
	tokens_.push_back(Token("root", 0));
	tokens_.push_back(Token(";", 0));
	Parser<Token>::iterator it = tokens_.begin();
	location_.set_root(it);
	EXPECT_EQ(location_.root(), "root");
}
TEST_F(LocationTests, root_doubleval)
{
	std::string root("root");
	tokens_.push_back(Token(root, 0));
	tokens_.push_back(Token(root, 0));
	tokens_.push_back(Token(";", 0));
	Parser<Token>::iterator it = tokens_.begin();
	EXPECT_THROW(location_.set_root(it), std::exception);
}

TEST_F(LocationTests, root_noval)  // ? or set to "./" ?
{
	tokens_.push_back(Token(";", 0));
	Parser<Token>::iterator it = tokens_.begin();
	EXPECT_NO_THROW(location_.set_root(it));
	EXPECT_EQ(location_.root(), "");
	// EXPECT_THROW(location_.set_root(it), std::exception);
}

TEST_F(LocationTests, index_one)
{
	std::string index("index.html");
	tokens_.push_back(Token(index, 0));
	tokens_.push_back(Token(";", 0));
	Parser<Token>::iterator it = tokens_.begin();
	location_.set_index(it);
	EXPECT_EQ(location_.index()[0], index);
}
TEST_F(LocationTests, index_two)
{
	std::string index[] = {"index.html", "index.php"};
	tokens_.push_back(Token(index[0], 0));
	tokens_.push_back(Token(index[1], 0));
	tokens_.push_back(Token(";", 0));
	Parser<Token>::iterator it = tokens_.begin();
	location_.set_index(it);
	EXPECT_EQ(location_.index()[0], index[0]);
	EXPECT_EQ(location_.index()[1], index[1]);
}
TEST_F(LocationTests, index_three)
{
	std::string index[] = {"index.html", "index.php", "index.js"};
	tokens_.push_back(Token(index[0], 0));
	tokens_.push_back(Token(index[1], 0));
	tokens_.push_back(Token(index[2], 0));
	tokens_.push_back(Token(";", 0));
	Parser<Token>::iterator it = tokens_.begin();
	location_.set_index(it);
	EXPECT_EQ(location_.index()[0], index[0]);
	EXPECT_EQ(location_.index()[1], index[1]);
	EXPECT_EQ(location_.index()[2], index[2]);
}
TEST_F(LocationTests, index_noval)
{
	tokens_.push_back(Token(";", 0));
	Parser<Token>::iterator it = tokens_.begin();
	location_.set_index(it);
	EXPECT_TRUE(location_.index().empty());
}

TEST_F(LocationTests, limit_except_none)
{
	EXPECT_TRUE(location_.allow("GET"));
	EXPECT_TRUE(location_.allow("POST"));
	EXPECT_TRUE(location_.allow("PUT"));
	EXPECT_FALSE(location_.allow("OPTIONS"));
	EXPECT_TRUE(location_.allow("DELETE"));
	EXPECT_TRUE(location_.allow("HEAD"));
	EXPECT_FALSE(location_.allow("PATCH"));
	EXPECT_FALSE(location_.allow("TRACE"));
	EXPECT_FALSE(location_.allow("BLABLA"));
}
TEST_F(LocationTests, limit_except_empty)
{
	tokens_.push_back(Token(";", 0));
	Parser<Token>::iterator it = tokens_.begin();
	location_.set_limit_except(it);
	EXPECT_FALSE(location_.allow("GET"));
	EXPECT_FALSE(location_.allow("POST"));
	EXPECT_FALSE(location_.allow("PUT"));
	EXPECT_FALSE(location_.allow("OPTIONS"));
	EXPECT_FALSE(location_.allow("DELETE"));
	EXPECT_FALSE(location_.allow("HEAD"));
	EXPECT_FALSE(location_.allow("PATCH"));
	EXPECT_FALSE(location_.allow("TRACE"));
	EXPECT_FALSE(location_.allow("BLABLA"));
}
TEST_F(LocationTests, limit_except_one)
{
	tokens_.push_back(Token("GET", 0));
	tokens_.push_back(Token(";", 0));
	Parser<Token>::iterator it = tokens_.begin();
	location_.set_limit_except(it);
	EXPECT_TRUE(location_.allow("GET"));
	EXPECT_FALSE(location_.allow("POST"));
	EXPECT_FALSE(location_.allow("PUT"));
	EXPECT_FALSE(location_.allow("OPTIONS"));
	EXPECT_FALSE(location_.allow("DELETE"));
	EXPECT_FALSE(location_.allow("HEAD"));
	EXPECT_FALSE(location_.allow("PATCH"));
	EXPECT_FALSE(location_.allow("TRACE"));
	EXPECT_FALSE(location_.allow("BLABLA"));
}
TEST_F(LocationTests, limit_except_invalidMethod)
{
	tokens_.push_back(Token("GET", 0));
	tokens_.push_back(Token("NOPOST", 0));
	tokens_.push_back(Token(";", 0));
	Parser<Token>::iterator it = tokens_.begin();
	EXPECT_THROW(location_.set_limit_except(it), std::exception);
}

TEST_F(LocationTests, upload_main)
{
	tokens_.push_back(Token("/upload", 0));
	tokens_.push_back(Token(";", 0));
	Parser<Token>::iterator it = tokens_.begin();
	location_.set_upload(it);
	EXPECT_EQ(location_.upload(), "/upload");
}
TEST_F(LocationTests, upload_noSlash)
{
	tokens_.push_back(Token("upload", 0));
	tokens_.push_back(Token(";", 0));
	Parser<Token>::iterator it = tokens_.begin();
	location_.set_upload(it);
	EXPECT_EQ(location_.upload(), "/upload");
}
TEST_F(LocationTests, upload_doubleval)
{
	std::string upload("upload");
	tokens_.push_back(Token(upload, 0));
	tokens_.push_back(Token(upload, 0));
	tokens_.push_back(Token(";", 0));
	Parser<Token>::iterator it = tokens_.begin();
	EXPECT_THROW(location_.set_upload(it), std::exception);
}

TEST_F(LocationTests, upload_noval)	 // ? or set to "./" ?
{
	tokens_.push_back(Token(";", 0));
	tokens_.push_back(Token("}", 0));
	Parser<Token>::iterator it = tokens_.begin();
	EXPECT_THROW(location_.set_upload(it), std::exception);
}

// void cgi(Parser<Token>::iterator &it);

TEST_F(LocationTests, cgi_main)
{
	tokens_.push_back(Token("php", 0));
	tokens_.push_back(Token("/cgi-bin/php-cgi", 0));
	tokens_.push_back(Token(";", 0));
	Parser<Token>::iterator it = tokens_.begin();
	location_.set_cgi(it);
	EXPECT_TRUE(location_.hasCGI("php"));
	EXPECT_EQ(location_.CGIpath("php"), "/cgi-bin/php-cgi");
}
TEST_F(LocationTests, cgi_main_noSlash)
{
	tokens_.push_back(Token("php", 0));
	tokens_.push_back(Token("cgi-bin/php-cgi", 0));
	tokens_.push_back(Token(";", 0));
	Parser<Token>::iterator it = tokens_.begin();
	location_.set_cgi(it);
	EXPECT_TRUE(location_.hasCGI("php"));
	EXPECT_EQ(location_.CGIpath("php"), "/cgi-bin/php-cgi");
}
TEST_F(LocationTests, cgi_same)
{
	tokens_.push_back(Token("php", 0));
	tokens_.push_back(Token("/cgi-bin/php-cgi", 0));
	tokens_.push_back(Token(";", 0));
	Parser<Token>::iterator it = tokens_.begin();
	location_.set_cgi(it);
	tokens_[1].value_ = "php-cgi";
	it = tokens_.begin();
	location_.set_cgi(it);
	EXPECT_TRUE(location_.hasCGI("php"));
	EXPECT_EQ(location_.CGIpath("php"), "/php-cgi");
}

TEST_F(LocationTests, cgi_two)
{
	tokens_.push_back(Token("php", 0));
	tokens_.push_back(Token("/cgi-bin/php-cgi", 0));
	tokens_.push_back(Token(";", 0));
	Parser<Token>::iterator it = tokens_.begin();
	location_.set_cgi(it);
	tokens_[0].value_ = "py";
	tokens_[1].value_ = "py-cgi";
	it = tokens_.begin();
	location_.set_cgi(it);
	EXPECT_TRUE(location_.hasCGI("php"));
	EXPECT_EQ(location_.CGIpath("php"), "/cgi-bin/php-cgi");
	EXPECT_TRUE(location_.hasCGI("py"));
	EXPECT_EQ(location_.CGIpath("py"), "/py-cgi");
}

TEST_F(LocationTests, cgi_tooManyValues)
{
	tokens_.push_back(Token("php", 0));
	tokens_.push_back(Token("/cgi-bin", 0));
	tokens_.push_back(Token("/php-cgi", 0));
	tokens_.push_back(Token(";", 0));
	Parser<Token>::iterator it = tokens_.begin();
	EXPECT_THROW(location_.set_cgi(it), std::exception);
}
TEST_F(LocationTests, cgi_empty)
{
	tokens_.push_back(Token(";", 0));
	tokens_.push_back(Token("}", 0));
	Parser<Token>::iterator it = tokens_.begin();
	EXPECT_THROW(location_.set_cgi(it), std::exception);
}

TEST_F(LocationTests, directives)
{
	tokens_.push_back(Token("cgi-bin", 0));
	tokens_.push_back(Token("/cgi_bin", 0));
	tokens_.push_back(Token(";", 0));
	Parser<Token>::iterator it = tokens_.begin();
	location_.set_directive(it);
	EXPECT_EQ(location_.directive("cgi-bin"), "/cgi_bin");
}

TEST_F(LocationTests, matches)
{
	tokens_.push_back(Token("/path1", 0));
	tokens_.push_back(Token("{", 0));
	tokens_.push_back(Token("}", 0));
	Parser<Token>::iterator it = tokens_.begin();
	location_.location(it);

	EXPECT_FALSE(location_.exactMatch("/path1"));
	EXPECT_EQ(6, location_.matches("/path1"));
}

TEST_F(LocationTests, client_max_body_size)
{
	tokens_.push_back(Token("32000", 0));
	tokens_.push_back(Token(";", 0));
	Parser<Token>::iterator it = tokens_.begin();
	location_.set_client_max_body_size(it);
	EXPECT_EQ(location_.maxBodySize(), 32000);
}
TEST_F(LocationTests, client_max_body_size_none)
{
	EXPECT_EQ(location_.maxBodySize(), -1);
}
TEST_F(LocationTests, client_max_body_size_zero)
{
	tokens_.push_back(Token("0", 0));
	tokens_.push_back(Token(";", 0));
	Parser<Token>::iterator it = tokens_.begin();
	location_.set_client_max_body_size(it);
	EXPECT_EQ(location_.maxBodySize(), 0);
}
TEST_F(LocationTests, client_max_body_size_100M)
{
	tokens_.push_back(Token("100M", 0));
	tokens_.push_back(Token(";", 0));
	Parser<Token>::iterator it = tokens_.begin();
	EXPECT_THROW(location_.set_client_max_body_size(it), std::exception);
}
TEST_F(LocationTests, client_max_body_size_neg)
{
	tokens_.push_back(Token("-100", 0));
	tokens_.push_back(Token(";", 0));
	Parser<Token>::iterator it = tokens_.begin();
	EXPECT_THROW(location_.set_client_max_body_size(it), std::exception);
}
TEST_F(LocationTests, client_max_body_size_alpha)
{
	tokens_.push_back(Token("Onehundred", 0));
	tokens_.push_back(Token(";", 0));
	Parser<Token>::iterator it = tokens_.begin();
	EXPECT_THROW(location_.set_client_max_body_size(it), std::exception);
}
