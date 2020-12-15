#include <gtest/gtest.h>

#include <Config/ServerList.hpp>
using namespace Config;

struct ServerListTests : public ::testing::Test
{
	class ServerListMock : public ServerList
	{
	public:
		ServerListMock() : ServerList() {}
		Server &add(Server toAdd)
		{
			return ServerList::add(toAdd);
		}
		Server const &match(Listen toMatch, std::string const &hostField) const
		{
			return ServerList::match(toMatch, hostField);
		}
		int matchByHost(std::vector<int> matches, std::string hostName) const
		{
			return ServerList::matchByHost(matches, hostName);
		}
	};
	ServerListMock mock;
};

TEST_F(ServerListTests, uniqueListen)
{
	std::vector<Token> tokens{{"8080", 0}, {";", 0}};
	std::vector<Token>::iterator it = tokens.begin();
	mock.add(Server());

	mock.add(Server());
	EXPECT_EQ(1, mock.uniqueListens().size());
	mock.add(Server()).set_listen(it);
	EXPECT_EQ(2, mock.uniqueListens().size());
}
TEST_F(ServerListTests, uniqueListen2)
{
	Server zero;
	Server one;
	Server two;
	Server three;
	Server four;

	std::vector<Token> tokens{{"{", 0},
							  {"listen", 0},
							  {"192.0.0.1", 0},
							  {";", 0},
							  {"}", 0}};
	std::vector<Token> tokens2{{"{", 0},
							   {"listen", 0},
							   {"10.0.0.10:8080", 0},
							   {";", 0},
							   {"}", 0}};
	std::vector<Token> tokens3{{"{", 0},
							   {"listen", 0},
							   {"8080", 0},
							   {";", 0},
							   {"}", 0}};
	std::vector<Token> tokens4{{"{", 0},
							   {"listen", 0},
							   {"6.6.6.6:666", 0},
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

	mock.add(zero);
	mock.add(one);
	mock.add(two);
	mock.add(three);
	mock.add(four);
	EXPECT_EQ(3, mock.uniqueListens().size());
	std::vector<std::pair<in_addr_t, u_int16_t> > res{{inet_addr("0.0.0.0"), htons(80)},
													  {inet_addr("0.0.0.0"), htons(8080)},
													  {inet_addr("6.6.6.6"), htons(666)}};
	EXPECT_EQ(res, mock.uniqueListens());
}

TEST_F(ServerListTests, matching)
{
	Server one;
	Server two;
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
	Server three;
	three.server(it);
	two.server(it2);
	mock.add(one);
	mock.add(two);
	mock.add(three);

	EXPECT_FALSE(three.listen() == mock.match(Listen(), "").listen());
	EXPECT_EQ(one.listen(), mock.match(Listen(), "").listen());
	EXPECT_EQ(three.listen(), mock.match(Listen("0.0.0.0", 8080), "").listen());
	EXPECT_EQ(1, mock.matchByHost({0, 1}, "localhost"));
}
TEST_F(ServerListTests, matchByHost)
{
	Server zero;
	Server one;
	Server two;
	Server three;

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

	mock.add(zero);
	mock.add(one);
	mock.add(two);
	mock.add(three);

	EXPECT_EQ(2, mock.matchByHost({2, 3}, "none"));
	EXPECT_EQ(3, mock.matchByHost({2, 3}, "www.localhost"));
	EXPECT_EQ(2, mock.matchByHost({2, 3}, "localhost.com"));
	EXPECT_EQ(3, mock.matchByHost({2, 3}, "localhost"));
	EXPECT_EQ(0, mock.matchByHost({0, 2, 3}, ""));
	EXPECT_EQ(1, mock.matchByHost({0, 1, 2, 3}, "localhost"));
	EXPECT_EQ(0, mock.matchByHost({0, 1, 2, 3}, "local"));
	EXPECT_EQ(2, mock.matchByHost({0, 1, 2, 3}, "localhost.com"));
}

TEST_F(ServerListTests, workers0)
{
	std::vector<Token> tokens{{"workers", 0},
							  {"0", 0},
							  {";", 0}};
	std::vector<Token>::iterator it = tokens.begin();
	set_workers(++it);
	EXPECT_EQ(1, Server::WORKER_PROCESSES);
	EXPECT_EQ(";", it->value_);
}
TEST_F(ServerListTests, workers2)
{
	std::vector<Token> tokens{{"workers", 0},
							  {"2", 0},
							  {";", 0}};
	std::vector<Token>::iterator it = tokens.begin();
	set_workers(++it);
	EXPECT_EQ(2, Server::WORKER_PROCESSES);
	EXPECT_EQ(";", it->value_);
}
TEST_F(ServerListTests, workersTooSmall)
{
	std::vector<Token> tokens{{"workers", 0},
							  {"-1", 0},
							  {";", 0}};
	std::vector<Token>::iterator it = tokens.begin();
	EXPECT_ANY_THROW(set_workers(++it));
}
TEST_F(ServerListTests, workersNonNumeric)
{
	std::vector<Token> tokens{{"workers", 0},
							  {"hej", 0},
							  {";", 0}};
	std::vector<Token>::iterator it = tokens.begin();
	EXPECT_ANY_THROW(set_workers(++it));
}
TEST_F(ServerListTests, workersDoubleValue)
{
	std::vector<Token> tokens{{"workers", 0},
							  {"2", 0},
							  {"10", 0},
							  {";", 0}};
	std::vector<Token>::iterator it = tokens.begin();
	EXPECT_ANY_THROW(set_workers(++it));
}
TEST_F(ServerListTests, workersNoValue)
{
	std::vector<Token> tokens{{"workers", 0},
							  {";", 0}};
	std::vector<Token>::iterator it = tokens.begin();
	EXPECT_ANY_THROW(set_workers(++it));
}
TEST_F(ServerListTests, plugins)
{
	Server::PLUGINS["utf-8"] = false;
	Server::PLUGINS["gzip"] = false;
	std::vector<Token> tokens{{"utf-8", 0},
							  {"on", 0},
							  {";", 0}};
	std::vector<Token>::iterator it = tokens.begin();
	set_plugins(it);
	EXPECT_TRUE(Server::PLUGINS["utf-8"]);
	EXPECT_FALSE(Server::PLUGINS["gzip"]);
}
TEST_F(ServerListTests, pluginsOff)
{
	Server::PLUGINS["utf-8"] = false;
	Server::PLUGINS["gzip"] = false;
	std::vector<Token> tokens{{"utf-8", 0},
							  {"off", 0},
							  {";", 0}};
	std::vector<Token>::iterator it = tokens.begin();
	set_plugins(it);
	EXPECT_FALSE(Server::PLUGINS["utf-8"]);
	EXPECT_FALSE(Server::PLUGINS["gzip"]);
}
TEST_F(ServerListTests, pluginsNoValue)
{
	Server::PLUGINS["utf-8"] = false;
	Server::PLUGINS["gzip"] = false;
	std::vector<Token> tokens{{"utf-8", 0},
							  {";", 0}};
	std::vector<Token>::iterator it = tokens.begin();
	EXPECT_ANY_THROW(set_plugins(it));
}
TEST_F(ServerListTests, pluginsDoubleValue)
{
	Server::PLUGINS["utf-8"] = false;
	Server::PLUGINS["gzip"] = false;
	std::vector<Token> tokens{{"utf-8", 0},
							  {"off", 0},
							  {"on", 0},
							  {";", 0}};
	std::vector<Token>::iterator it = tokens.begin();
	EXPECT_ANY_THROW(set_plugins(it));
}
