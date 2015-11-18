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

class shortGems : public ::testing::Test {
  public:
    shortGems() : data_(1, 2, 3, 4) {} // Note: not BLACK or YELLOW)
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

