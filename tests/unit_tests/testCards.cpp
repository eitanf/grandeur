//
// Unit test for Card class
// Created by eitan on 11/19/15.
//


#include "gtest/gtest.h"
#include "card.h"

#include <algorithm>

using namespace grandeur;
using namespace std;

TEST(cardTests, correctCost)
{
    // No card should have any negative cost
    EXPECT_FALSE(any_of(begin(g_deck), end(g_deck), [](const Card& card){
        return card.cost_.hasNegatives();
    }));

    // No card should cost any yellow gems:
    EXPECT_FALSE(any_of(begin(g_deck), end(g_deck), [](const Card& card){
        return (card.cost_.getCount(YELLOW) > 0);
    }));
}

// Check that the original deck size conforms to original specs:
TEST(cardTests, correctSize)
{
    EXPECT_EQ(deckCount(LOW, g_deck), 40);
    EXPECT_EQ(deckCount(MEDIUM, g_deck), 30);
    EXPECT_EQ(deckCount(HIGH, g_deck), 20);
}