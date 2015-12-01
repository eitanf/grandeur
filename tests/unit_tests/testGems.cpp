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

TEST(gemsTest, actualCost)
{
    Gems gems({ 1, 2, 0, 3, 0, 2 });

    // Nothing costs nothing.
    EXPECT_EQ(gems.actualCost(Gems()), Gems());
    // No discounts, no shortages:
    EXPECT_EQ(gems.actualCost(Gems({1, 1, 0, 3, 0})), Gems({1, 1, 0, 3, 0, 0}));
    // Discounts:
    EXPECT_EQ(gems.actualCost(Gems({1, 2, -1, 3, -5})), Gems({1, 2, 0, 3, 0, 0}));
    // Some shortage
    EXPECT_EQ(gems.actualCost(Gems({2, 2, 0, 3, 0})), Gems({1, 2, 0, 3, 0, 1}));
    // Complete shortage:
    EXPECT_EQ(gems.actualCost(Gems({2, 2, 0, 3, 1})), Gems({1, 2, 0, 3, 0, 2}));
    // Over shortage:
    EXPECT_EQ(gems.actualCost(Gems({4, 0, 0, 0, 0})), Gems({1, 0, 0, 0, 0, 3}));
    // Shortage everywhere:
    EXPECT_EQ(gems.actualCost(Gems({2, 3, 1, 4, 1})), Gems({1, 2, 0, 3, 0, 5}));
    // Shortage and discounts:
    EXPECT_EQ(gems.actualCost(Gems({2, 3, -1, -5, -2})), Gems({1, 2, 0, 0, 0, 2}));
    // Too much shortage despite too much discount:
    EXPECT_EQ(gems.actualCost(Gems({5, -5, -5, -5})), Gems({1, 0, 0, 0, 0, 4}));
}

TEST(gemsTest, negativeDiscountDoesntAffectActualCost)
{
    Gems gems(0, 2, 0, 0, 0, 3);

    EXPECT_EQ(gems.actualCost(Gems({3, 2, 1, 0, 0, 0})), Gems({0, 2, 0, 0, 0, 4}));
    EXPECT_EQ(gems.actualCost(Gems({3, 2, 1, -1, -1, 0})), Gems({0, 2, 0, 0, 0, 4}));
}


class shortGems : public ::testing::Test {
  public:
    shortGems() : data_(1, 2, 3, -4) {} // Note: no BLACK or YELLOW)
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
    EXPECT_EQ(data_.positiveColors(), 3);
}

TEST_F(shortGems, detectNegative)
{
    EXPECT_TRUE(data_.hasNegatives());
    EXPECT_FALSE(Gems().hasNegatives());
    EXPECT_FALSE(Gems(1,2,3,4,5,6).hasNegatives());
}

TEST_F(shortGems, verifyMaxQuantity) {
    ASSERT_EQ(data_.getCount(data_.maxColor()), 3);
}