#include <gtest/gtest.h>

#include <Config/Listen.hpp>

TEST(ListenTests, defaultConstructor)
{
	Listen listen1;

	EXPECT_EQ(listen1.ip(), "0.0.0.0");
	EXPECT_EQ(listen1.port(), 80);
}
TEST(ListenTests, constructor)
{
	Listen listen1("192.0.0.1", 8080);

	EXPECT_EQ(listen1.ip(), "192.0.0.1");
	EXPECT_EQ(listen1.port(), 8080);
}
// TEST(ListenTests, fdConstructor)
// {
// 	Listen listen1(2);

// 	EXPECT_EQ(listen1.ip(), "0.0.0.0");
// 	EXPECT_EQ(listen1.port(), 0);
// }
TEST(ListenTests, copyConstructor)
{
	Listen listen1;
	Listen listen2(listen1);
	Listen listen3("192.0.0.1", 8080);

	EXPECT_EQ(listen1.ip(), "0.0.0.0");
	EXPECT_EQ(listen1.port(), 80);
	EXPECT_EQ(listen2.ip(), "0.0.0.0");
	EXPECT_EQ(listen2.port(), 80);
	EXPECT_EQ(listen3.ip(), "192.0.0.1");
	EXPECT_EQ(listen3.port(), 8080);
	listen1 = listen3;
	EXPECT_EQ(listen1.ip(), "192.0.0.1");
	EXPECT_EQ(listen1.port(), 8080);
}
TEST(ListenTests, eqalityOperator)
{
	Listen listen1;
	Listen listen2("0.0.0.0", 80);
	Listen listen3("192.0.0.1", 80);
	Listen listen4("192.0.0.1", 8080);

	EXPECT_EQ(listen1, listen2);
	EXPECT_EQ(listen1, listen3);
	EXPECT_FALSE(listen1 == listen4);
	EXPECT_FALSE(listen3 == listen4);
	EXPECT_FALSE(listen1 != listen2);
	EXPECT_FALSE(listen1 != listen3);
	EXPECT_TRUE(listen1 != listen4);
	EXPECT_TRUE(listen3 != listen4);
}
