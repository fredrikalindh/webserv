#include <gtest/gtest.h>

#include <Config/Server.hpp>

using namespace Config;

struct ServerTests : public ::testing::Test
{
	class ConfigMock : public Config::Server
	{
	public:
		ConfigMock() : Server() {}
		Server &add(Server toAdd)
		{
			return Server::add(toAdd);
		}
		Server const &match(Listen toMatch, std::string const &hostField) const
		{
			return Server::match(toMatch, hostField);
		}
		int matchByHost(std::vector<int> matches, std::string hostName) const
		{
			int index = Server::matchByHost(matches, hostName);
			return index;
		}
		bool dir(Parser<Token>::iterator &it)
		{
			return Server::dir(it);
		}
		void server(Parser<Token>::iterator &it)
		{
			Server::server(it);
		}
		void set_listen(Parser<Token>::iterator &it)
		{
			Server::set_listen(it);
		}
		void set_server_name(Parser<Token>::iterator &it)
		{
			Server::set_server_name(it);
		}
		void set_location(Parser<Token>::iterator &it)
		{
			Server::set_location(it);
		}
		void set_error_page(Parser<Token>::iterator &it)
		{
			Server::set_error_page(it);
		}
		void set_directive(Parser<Token>::iterator &it)
		{
			Server::set_directive(it);
		}
		void clear()
		{
			Server::configs_.clear();
		}
	};

	ConfigMock config;
	std::vector<Token> tokens_;

	void addTokens(std::string *begin, std::string *end)
	{
		Token tmp;
		while (begin != end)
		{
			tmp = Token(*begin, 0);
			tokens_.push_back(tmp);
			begin++;
		}  // while (begin != end)
		   // 	//     tokens_.push_back(Token(*begin++, 0));
		   // 	//     tokens_.push_back(Token(*begin++, 0));
	}
	// 	virtual void SetUp()
	// 	{
	// 	}
	// 	virtual void TearDown()
	// 	{
	// 	}
};

TEST_F(ServerTests, uniqueListen)
{
	std::vector<Token> tokens{{"8080", 0}, {";", 0}};
	std::vector<Token>::iterator it = tokens.begin();
	config.add(Server());

	ConfigMock mock;
	mock.set_listen(it);
	config.add(Server());

	EXPECT_EQ(1, config.uniqueListens().size());
	config.add(mock);
	EXPECT_EQ(2, config.uniqueListens().size());
}
TEST_F(ServerTests, uniqueListen2)
{
	ConfigMock zero;
	ConfigMock one;
	ConfigMock two;
	ConfigMock three;
	ConfigMock four;

	std::vector<Token> tokens{{"{", 0},
							  {"listen", 0},
							  {"80", 0},
							  {";", 0},
							  {"}", 0}};
	std::vector<Token> tokens2{{"{", 0},
							   {"listen", 0},
							   {"8080", 0},
							   {";", 0},
							   {"}", 0}};
	std::vector<Token> tokens3{{"{", 0},
							   {"listen", 0},
							   {"8080", 0},
							   {";", 0},
							   {"}", 0}};
	std::vector<Token> tokens4{{"{", 0},
							   {"listen", 0},
							   {"666", 0},
							   {";", 0},
							   {"}", 0}};
	std::vector<Token>::iterator it = tokens.begin();
	one.server(it);
	it = tokens2.begin();
	two.server(it);
	it = tokens3.begin();
	three.server(it);
	it = tokens4.begin();
	four.server(it);

	config.add(zero);
	config.add(one);
	config.add(two);
	config.add(three);
	config.add(four);
	EXPECT_EQ(3, config.uniqueListens().size());
	std::vector<u_int16_t> res{htons(80),
							   htons(8080),
							   htons(666)};
	EXPECT_EQ(res, config.uniqueListens());
}

TEST_F(ServerTests, matchingServer)
{
	config.clear();
	ConfigMock one;
	ConfigMock two;
	std::vector<Token> tokens{{"{", 0},
							  {"server_name", 0},
							  {"localhost", 0},
							  {";", 0},
							  {"listen", 0},
							  {"8080", 0},
							  {";", 0},
							  {"}", 0}};
	std::vector<Token> tokens2{{"{", 0},
							   {"server_name", 0},
							   {"localhost", 0},
							   {";", 0},
							   {"listen", 0},
							   {"80", 0},
							   {";", 0},
							   {"}", 0}};
	std::vector<Token>::iterator it = tokens.begin();
	std::vector<Token>::iterator it2 = tokens2.begin();
	ConfigMock three;
	three.server(it);
	two.server(it2);
	config.add(one);
	config.add(two);
	config.add(three);

	EXPECT_FALSE(three.listen() == config.match(Listen(80), "").listen());
	EXPECT_EQ(one.listen(), config.match(Listen(80), "").listen());
	EXPECT_EQ(three.listen(), config.match(Listen(8080), "").listen());
	EXPECT_EQ(1, config.matchByHost({0, 1}, "localhost"));
}
TEST_F(ServerTests, matchByHostname)
{
	config.clear();
	ConfigMock zero;
	ConfigMock one;
	ConfigMock two;
	ConfigMock three;

	std::vector<Token> tokens{{"{", 0},
							  {"server_name", 0},
							  {"localhost", 0},
							  {";", 0},
							  {"}", 0}};
	std::vector<Token> tokens2{{"{", 0},
							   {"server_name", 0},
							   {"localhost*", 0},
							   {";", 0},
							   {"}", 0}};
	std::vector<Token> tokens3{{"{", 0},
							   {"server_name", 0},
							   {"*localhost", 0},
							   {";", 0},
							   {"}", 0}};
	std::vector<Token>::iterator it = tokens.begin();
	one.server(it);
	it = tokens2.begin();
	two.server(it);
	it = tokens3.begin();
	three.server(it);

	config.add(zero);
	config.add(one);
	config.add(two);
	config.add(three);

	EXPECT_EQ(2, config.matchByHost({2, 3}, "none"));
	EXPECT_EQ(3, config.matchByHost({2, 3}, "www.localhost"));
	EXPECT_EQ(2, config.matchByHost({2, 3}, "localhost.com"));
	EXPECT_EQ(3, config.matchByHost({2, 3}, "localhost"));
	EXPECT_EQ(0, config.matchByHost({0, 2, 3}, ""));
	EXPECT_EQ(1, config.matchByHost({0, 1, 2, 3}, "localhost"));
	EXPECT_EQ(0, config.matchByHost({0, 1, 2, 3}, "local"));
	EXPECT_EQ(2, config.matchByHost({0, 1, 2, 3}, "localhost.com"));
}
#ifdef BONUS

TEST_F(ServerTests, workers0)
{
	std::vector<Token> tokens{{"workers", 0},
							  {"0", 0},
							  {";", 0}};
	std::vector<Token>::iterator it = tokens.begin();
	set_workers(++it);
	EXPECT_EQ(1, Server::WORKER_PROCESSES);
	EXPECT_EQ(";", it->value_);
}
TEST_F(ServerTests, workers2)
{
	std::vector<Token> tokens{{"workers", 0},
							  {"2", 0},
							  {";", 0}};
	std::vector<Token>::iterator it = tokens.begin();
	set_workers(++it);
	EXPECT_EQ(2, Server::WORKER_PROCESSES);
	EXPECT_EQ(";", it->value_);
}
TEST_F(ServerTests, workersTooSmall)
{
	std::vector<Token> tokens{{"workers", 0},
							  {"-1", 0},
							  {";", 0}};
	std::vector<Token>::iterator it = tokens.begin();
	EXPECT_ANY_THROW(set_workers(++it));
}
TEST_F(ServerTests, workersNonNumeric)
{
	std::vector<Token> tokens{{"workers", 0},
							  {"hej", 0},
							  {";", 0}};
	std::vector<Token>::iterator it = tokens.begin();
	EXPECT_ANY_THROW(set_workers(++it));
}
TEST_F(ServerTests, workersDoubleValue)
{
	std::vector<Token> tokens{{"workers", 0},
							  {"2", 0},
							  {"10", 0},
							  {";", 0}};
	std::vector<Token>::iterator it = tokens.begin();
	EXPECT_ANY_THROW(set_workers(++it));
}
TEST_F(ServerTests, workersNoValue)
{
	std::vector<Token> tokens{{"workers", 0},
							  {";", 0}};
	std::vector<Token>::iterator it = tokens.begin();
	EXPECT_ANY_THROW(set_workers(++it));
}
#endif
// TEST_F(ServerTests, plugins)
// {
// 	Server::PLUGINS["utf-8"] = false;
// 	Server::PLUGINS["gzip"] = false;
// 	std::vector<Token> tokens{{"utf-8", 0},
// 							  {"on", 0},
// 							  {";", 0}};
// 	std::vector<Token>::iterator it = tokens.begin();
// 	set_plugins(it);
// 	EXPECT_TRUE(Server::PLUGINS["utf-8"]);
// 	EXPECT_FALSE(Server::PLUGINS["gzip"]);
// }
// TEST_F(ServerTests, pluginsOff)
// {
// 	Server::PLUGINS["utf-8"] = false;
// 	Server::PLUGINS["gzip"] = false;
// 	std::vector<Token> tokens{{"utf-8", 0},
// 							  {"off", 0},
// 							  {";", 0}};
// 	std::vector<Token>::iterator it = tokens.begin();
// 	set_plugins(it);
// 	EXPECT_FALSE(Server::PLUGINS["utf-8"]);
// 	EXPECT_FALSE(Server::PLUGINS["gzip"]);
// }
// TEST_F(ServerTests, pluginsNoValue)
// {
// 	Server::PLUGINS["utf-8"] = false;
// 	Server::PLUGINS["gzip"] = false;
// 	std::vector<Token> tokens{{"utf-8", 0},
// 							  {";", 0}};
// 	std::vector<Token>::iterator it = tokens.begin();
// 	EXPECT_ANY_THROW(set_plugins(it));
// }
// TEST_F(ServerTests, pluginsDoubleValue)
// {
// 	Server::PLUGINS["utf-8"] = false;
// 	Server::PLUGINS["gzip"] = false;
// 	std::vector<Token> tokens{{"utf-8", 0},
// 							  {"off", 0},
// 							  {"on", 0},
// 							  {";", 0}};
// 	std::vector<Token>::iterator it = tokens.begin();
// 	EXPECT_ANY_THROW(set_plugins(it));
// }

TEST_F(ServerTests, defaultConstructor)
{
	Config::Server conf;
	EXPECT_EQ(conf.listen(), Listen(80));
	EXPECT_FALSE(conf.errorPageExists(400));
	EXPECT_FALSE(conf.exactMatch("localhost"));
}

TEST_F(ServerTests, server_full)
{
	std::string values[] = {"{", "location", "/", "{", "}", "listen", "80", ";", "server_name", "localhost", ";", "error_page", "404", "/404.html", ";", "client_max_body_size", "100", ";", "}"};
	addTokens(values, values + 19);

	Parser<Token>::iterator it = tokens_.begin();
	EXPECT_NO_THROW(config.server(it));
	EXPECT_EQ(config.listen(), Listen(80));
	EXPECT_FALSE(config.errorPageExists(400));
	EXPECT_TRUE(config.exactMatch("localhost"));
	ASSERT_TRUE(config.errorPageExists(404));
	EXPECT_EQ(config.errorPage(404), "/404.html");
}

TEST_F(ServerTests, server_badDir)
{
	std::string values[] = {"{", "badDir", "}"};
	addTokens(values, values + 3);
	Parser<Token>::iterator it = tokens_.begin();
	EXPECT_THROW(config.server(it), std::exception);
}

TEST_F(ServerTests, server_missingBracket)
{
	tokens_.push_back(Token("}", 0));
	Parser<Token>::iterator it = tokens_.begin();
	EXPECT_THROW(config.server(it), std::exception);
}

TEST_F(ServerTests, listen_ip_port)
{
	tokens_.push_back(Token("8080", 0));
	tokens_.push_back(Token(";", 0));
	Parser<Token>::iterator it = tokens_.begin();
	config.set_listen(it);
	EXPECT_EQ(config.listen(), Listen(8080));
}
TEST_F(ServerTests, listen_ip)
{
	tokens_.push_back(Token("80", 0));
	tokens_.push_back(Token(";", 0));
	Parser<Token>::iterator it = tokens_.begin();
	config.set_listen(it);
	EXPECT_EQ(config.listen(), Listen(80));
}
TEST_F(ServerTests, listen_port)
{
	tokens_.push_back(Token("8888", 0));
	tokens_.push_back(Token(";", 0));
	Parser<Token>::iterator it = tokens_.begin();
	config.set_listen(it);
	EXPECT_EQ(config.listen(), Listen(8888));
}
TEST_F(ServerTests, listen_port1)
{
	tokens_.push_back(Token(":8888", 0));
	tokens_.push_back(Token(";", 0));
	Parser<Token>::iterator it = tokens_.begin();
	EXPECT_THROW(config.set_listen(it), std::exception);
}
TEST_F(ServerTests, listen_fail0)
{
	tokens_.push_back(Token("0.0.0:80.0", 0));
	tokens_.push_back(Token(";", 0));
	Parser<Token>::iterator it = tokens_.begin();
	EXPECT_THROW(config.set_listen(it), std::exception);
}
TEST_F(ServerTests, listen_fail1)
{
	tokens_.push_back(Token("h.h.h.h:80", 0));
	tokens_.push_back(Token(";", 0));
	Parser<Token>::iterator it = tokens_.begin();
	EXPECT_THROW(config.set_listen(it), std::exception);
}
TEST_F(ServerTests, listen_fail2)
{
	tokens_.push_back(Token("0.0.0.0.:80", 0));
	tokens_.push_back(Token(";", 0));
	Parser<Token>::iterator it = tokens_.begin();
	EXPECT_THROW(config.set_listen(it), std::exception);
}
TEST_F(ServerTests, listen_ip_fail)
{
	tokens_.push_back(Token("i.will.not.work", 0));
	tokens_.push_back(Token(";", 0));
	Parser<Token>::iterator it = tokens_.begin();
	EXPECT_THROW(config.set_listen(it), std::exception);
}
TEST_F(ServerTests, listen_port_fail)
{
	tokens_.push_back(Token("hej", 0));
	tokens_.push_back(Token(";", 0));
	Parser<Token>::iterator it = tokens_.begin();
	EXPECT_THROW(config.set_listen(it), std::exception);
}
TEST_F(ServerTests, listen_port_fail2)
{
	tokens_.push_back(Token("-18", 0));
	tokens_.push_back(Token(";", 0));
	Parser<Token>::iterator it = tokens_.begin();
	EXPECT_THROW(config.set_listen(it), std::exception);
}
TEST_F(ServerTests, listen_port_fail3)
{
	tokens_.push_back(Token("180000", 0));
	tokens_.push_back(Token(";", 0));
	Parser<Token>::iterator it = tokens_.begin();
	EXPECT_THROW(config.set_listen(it), std::exception);
}

TEST_F(ServerTests, server_name)
{
	tokens_.push_back(Token("localhost", 0));
	tokens_.push_back(Token("www.localhost", 0));
	tokens_.push_back(Token(";", 0));
	Parser<Token>::iterator it = tokens_.begin();
	config.set_server_name(it);
	EXPECT_TRUE(config.exactMatch("localhost"));
	EXPECT_TRUE(config.exactMatch("www.localhost"));
	EXPECT_FALSE(config.exactMatch("www.localhost.com"));
}

TEST_F(ServerTests, server_name_noval)
{
	tokens_.push_back(Token(";", 0));
	Parser<Token>::iterator it = tokens_.begin();
	config.set_server_name(it);
	EXPECT_FALSE(config.exactMatch("www.localhost.com"));
	EXPECT_FALSE(config.exactMatch(""));
}
/**
 * TODO:
 * server_name wildcard match tests
 */

TEST_F(ServerTests, error_page)
{
	tokens_.push_back(Token("404", 0));
	tokens_.push_back(Token("400", 0));
	tokens_.push_back(Token("/40x.html", 0));
	tokens_.push_back(Token(";", 0));
	Parser<Token>::iterator it = tokens_.begin();
	config.set_error_page(it);
	EXPECT_TRUE(config.errorPageExists(404));
	EXPECT_EQ(config.errorPage(404), "/40x.html");
}

TEST_F(ServerTests, error_page_double)
{
	tokens_.push_back(Token("404", 0));
	tokens_.push_back(Token("400", 0));
	tokens_.push_back(Token("/40x.html", 0));
	tokens_.push_back(Token(";", 0));
	Parser<Token>::iterator it = tokens_.begin();
	config.set_error_page(it);
	ASSERT_TRUE(config.errorPageExists(404));
	ASSERT_TRUE(config.errorPageExists(400));
	EXPECT_EQ(config.errorPage(404), "/40x.html");
	EXPECT_EQ(config.errorPage(400), "/40x.html");
}
TEST_F(ServerTests, error_page_two)
{
	tokens_.push_back(Token("404", 0));
	tokens_.push_back(Token("/404.html", 0));
	tokens_.push_back(Token(";", 0));
	Parser<Token>::iterator it = tokens_.begin();
	config.set_error_page(it);
	tokens_[0].value_ = "400";
	tokens_[1].value_ = "/400.html";
	it = tokens_.begin();
	config.set_error_page(it);
	ASSERT_TRUE(config.errorPageExists(404));
	ASSERT_TRUE(config.errorPageExists(400));
	EXPECT_EQ(config.errorPage(404), "/404.html");
	EXPECT_EQ(config.errorPage(400), "/400.html");
}
TEST_F(ServerTests, error_page_bad_access)	// ? throw or not throw?
{
	EXPECT_FALSE(config.errorPageExists(404));
	EXPECT_ANY_THROW(config.errorPage(404));
	// std::string res = config.errorPage(404);
	// std::cout << " RESULT " << res << std::endl;
	// EXPECT_EQ(res, "");
	// EXPECT_ANY_THROW(res == "");
}
TEST_F(ServerTests, error_page_fail_lowcode)
{
	tokens_.push_back(Token("0", 0));
	tokens_.push_back(Token("/404.html", 0));
	tokens_.push_back(Token(";", 0));
	Parser<Token>::iterator it = tokens_.begin();
	EXPECT_THROW(config.set_error_page(it), std::exception);
}
TEST_F(ServerTests, error_page_fail_highcode)
{
	tokens_.push_back(Token("600", 0));
	tokens_.push_back(Token("/404.html", 0));
	tokens_.push_back(Token(";", 0));
	Parser<Token>::iterator it = tokens_.begin();
	EXPECT_THROW(config.set_error_page(it), std::exception);
}
TEST_F(ServerTests, error_page_fail_alphacode)
{
	tokens_.push_back(Token("400", 0));
	tokens_.push_back(Token("hej", 0));
	tokens_.push_back(Token("/404.html", 0));
	tokens_.push_back(Token(";", 0));
	Parser<Token>::iterator it = tokens_.begin();
	EXPECT_THROW(config.set_error_page(it), std::exception);
}
TEST_F(ServerTests, error_page_fail_noval)
{
	tokens_.push_back(Token(";", 0));
	Parser<Token>::iterator it = tokens_.begin();
	EXPECT_THROW(config.set_error_page(it), std::exception);
}
TEST_F(ServerTests, error_page_fail_nopath)
{
	tokens_.push_back(Token("404", 0));
	tokens_.push_back(Token(";", 0));
	Parser<Token>::iterator it = tokens_.begin();
	EXPECT_THROW(config.set_error_page(it), std::exception);
}
TEST_F(ServerTests, directives)
{
	tokens_.push_back(Token("cgi-bin", 0));
	tokens_.push_back(Token("/cgi_bin", 0));
	tokens_.push_back(Token(";", 0));
	Parser<Token>::iterator it = tokens_.begin();
	config.set_directive(it);
	EXPECT_EQ(config.directive("cgi-bin"), "/cgi_bin");
}

TEST_F(ServerTests, matchingServerNames)
{
	std::string values[] = {"{",
							"server_name",
							"localhost",
							"*localhost.com",
							"example.*",
							";",
							"}"};
	addTokens(values, values + 7);

	Parser<Token>::iterator it = tokens_.begin();
	EXPECT_NO_THROW(config.server(it));
	EXPECT_TRUE(config.exactMatch("localhost"));
	EXPECT_FALSE(config.exactMatch("localhostse"));
	EXPECT_FALSE(config.exactMatch("localhost.com"));
	EXPECT_EQ(13, config.wildcardMatch("www.localhost.com"));
	EXPECT_EQ(0, config.wildcardMatch("www.localhost.coms"));
	EXPECT_EQ(-8, config.wildcardMatch("example.com"));
	EXPECT_EQ(-8, config.wildcardMatch("example.se"));
	EXPECT_EQ(0, config.wildcardMatch("www.example.com"));
}
