//
// Unit test for Gems class
// Created by eitan on 11/18/15.
//

#include "gtest/gtest.h"
#include "gems.h"

using namespace grandeur;

TEST(gemsTest, ctorFromInitializerList)
{
    EXPECT_NO_THROW(Gems(1, 2, 3, -1, 5, 0));
    EXPECT_NO_THROW(Gems({1, 2, 3, -1, 5, 0}));
}

TEST(gemsTest, substraction)
{
    Gems x(1, 2, 3, 4), y(5, 4, 3, 2, 1), z;

    EXPECT_EQ(x - y, Gems(-4, -2, 0, 2, -1, 0));
    EXPECT_EQ(z, Gems(0, 0, 0, 0, 0, 0));
    EXPECT_EQ(y - x, Gems(4, 2, 0, -2, 1, 0));
    EXPECT_EQ(x - x, z);
    EXPECT_EQ(x - z, x);
    EXPECT_EQ(z - x, Gems(-1, -2, -3, -4));
    EXPECT_NE(x - y, y - x);
}

TEST(gemsTest, addition)
{
    Gems x(1, 2, 3, 4), y(5, 4, 3, 2, 1), z;

    EXPECT_EQ(x + x, Gems(2, 4, 6, 8, 0, 0));
    EXPECT_EQ(x + y, Gems(6, 6, 6, 6, 1));
    EXPECT_EQ(y + x, x + y);
    EXPECT_EQ(x + z - x, z);
    EXPECT_EQ(x + z, x);
    EXPECT_EQ(z + z, z);
}

class shortGems : public ::testing::Test {
  public:
    shortGems() : data_(1, 2, 3, -4) {} // Note: not BLACK or YELLOW)
    Gems data_;
};

TEST_F(shortGems, ctorZerosRemaining)
{
    EXPECT_NE(data_.getCount(WHITE), 0);
    EXPECT_NE(data_.getCount(TEAL), 0);
    EXPECT_NE(data_.getCount(GREEN), 0);
    EXPECT_NE(data_.getCount(RED), 0);
    EXPECT_EQ(data_.getCount(BLACK), 0);
    EXPECT_EQ(data_.getCount(YELLOW), 0);
}

TEST_F(shortGems, countTotalGems)
{
    EXPECT_EQ(data_.totalGems(), 2);
}

TEST_F(shortGems, countColors) {
    EXPECT_EQ(data_.totalColors(), 4);
}

TEST_F(shortGems, detectNegative)
{
    EXPECT_TRUE(data_.hasNegatives());
    EXPECT_FALSE(Gems().hasNegatives());
    EXPECT_FALSE(Gems(1,2,3,4,5,6).hasNegatives());
}

TEST_F(shortGems, verifyMaxQuantity) {
    ASSERT_EQ(data_.maxQuantity(), 3);
}