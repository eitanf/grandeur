//
// Unit tests for Board class
// Created by eitan on 11/19/15.
//


#include "gtest/gtest.h"
#include "board.h"
#include "card.h"

using namespace grandeur;

TEST(boardTests, ctor)
{
    Cards cards({ g_deck[0], g_deck[1], g_deck[2], g_deck[3],
                  g_deck[40], g_deck[41], g_deck[42], g_deck[43],
                  g_deck[70], g_deck[71], g_deck[72], g_deck[73] });
    Board board(2, cards);

    EXPECT_EQ(board.tableCards(), cards);

    EXPECT_TRUE(board.playerReserves(0).empty());
    EXPECT_TRUE(board.playerReserves(1).empty());

    EXPECT_TRUE(board.playerGems(0).empty());
    EXPECT_TRUE(board.playerGems(1).empty());

    EXPECT_TRUE(board.playerPrestige(0).empty());
    EXPECT_TRUE(board.playerPrestige(1).empty());

    EXPECT_EQ(board.playerPoints(0), 0);
    EXPECT_EQ(board.playerPoints(1), 0);

    EXPECT_EQ(board.remainingCards(LOW), deckCount(g_deck, LOW) - 4);
    EXPECT_EQ(board.remainingCards(MEDIUM), deckCount(g_deck, MEDIUM) - 4);
    EXPECT_EQ(board.remainingCards(HIGH), deckCount(g_deck, HIGH) - 4);
}


class MidGameBoard : public ::testing::Test {
  public:
    MidGameBoard()
       : initial_({ g_deck[0], g_deck[3], g_deck[4], g_deck[6],
                    g_deck[40], g_deck[41], g_deck[42], g_deck[43],
                    g_deck[70], g_deck[71], g_deck[72], g_deck[73] }),
         board_(nplayer_, initial_),
         hidden_()
    {
        int ok = LEGAL_MOVE;

        // Make some game moves (6 rounds):
        ok |= board_.takeGems(0, { 0, 2, 0, 0, 0 });
        ok |= board_.takeGems(1, { 1, 1, 1, 0, 0 });
        ok |= board_.takeGems(2, { 0, 0, 0, 2, 0 });
        assert(ok == LEGAL_MOVE);

        ok |= board_.takeGems(0, { 1, 1, 0, 1, 0 });
        ok |= board_.takeGems(1, { 1, 1, 0, 1, 0 });
        ok |= board_.takeGems(2, { 1, 0, 1, 1, 1 });
        assert(ok == LEGAL_MOVE);

        ok |= board_.reserveCard(0, { LOW, 7 }, hidden_[0]);
        ok |= board_.buyCard(1, { LOW, 0 }, hidden_[1], g_deck[12]);
        ok |= board_.buyCard(2, { LOW, 3 }, hidden_[2], g_deck[24]);
        assert(ok == LEGAL_MOVE);

        ok |= board_.buyCard(0, { LOW, 7 }, hidden_[0]);
        ok |= board_.takeGems(1, { 1, 1, 0, 1, 0});
        ok |= board_.takeGems(2, { 0, 1, 1, 1, 0});
        assert(ok == LEGAL_MOVE);

        ok |= board_.reserveCard(0, { MEDIUM, 42 }, hidden_[0]);
        ok |= board_.buyCard(1, { LOW, 24 }, hidden_[1], g_deck[32]);
        ok |= board_.takeGems(2, { 1, 1, 0, 1, 0});
        assert(ok == LEGAL_MOVE);

        ok |= board_.reserveCard(0, { LOW, 6 }, hidden_[1], g_deck[5]);
        ok |= board_.buyCard(1, { LOW, 32 }, hidden_[2], g_deck[1]);
        ok |= board_.buyCard(2, { LOW, 1 }, hidden_[2], NULL_CARD);
        assert(ok == LEGAL_MOVE);
    }

    static constexpr unsigned nplayer_ = 3;
    Cards initial_;
    Board board_;
    Cards hidden_[nplayer_];
};

// Verify that all accessors return the expected values
TEST_F(MidGameBoard, correctTableCards)
{
    auto& tcards = board_.tableCards();
    EXPECT_EQ(tcards.size(), initial_.size() - 1);
    EXPECT_EQ(deckCount(tcards, LOW), 3);
    EXPECT_EQ(deckCount(tcards, MEDIUM), 4);
    EXPECT_EQ(deckCount(tcards, HIGH), 4);
}

TEST_F(MidGameBoard, correctReserves)
{
    EXPECT_EQ(board_.playerReserves(0).size(), 1);
    EXPECT_EQ(board_.playerReserves(0).at(0).id_, CardID({ LOW, 6 }));
    EXPECT_EQ(board_.playerReserves(1).size(), 0);
    EXPECT_EQ(board_.playerReserves(2).size(), 0);

    EXPECT_EQ(hidden_[0].size(), 1);
    EXPECT_EQ(hidden_[0].at(0).id_, CardID({ MEDIUM, 43 }));
    EXPECT_EQ(hidden_[1].size(), 0);
    EXPECT_EQ(hidden_[2].size(), 1);
}

TEST_F(MidGameBoard, correctGems)
{
    EXPECT_EQ(board_.tableGems(),   Gems({ 3, 5, 5, 3, 5, 3 }));
    EXPECT_EQ(board_.playerGems(0), Gems({ 1, 0, 0, 1, 0, 2 }));
    EXPECT_EQ(board_.playerGems(1), Gems({ 0, 0, 0, 0, 0, 0 }));
    EXPECT_EQ(board_.playerGems(2), Gems({ 1, 0, 0, 1, 0, 0 }));
}

TEST_F(MidGameBoard, correctPrestige)
{
    EXPECT_EQ(board_.playerPrestige(0), Gems({ 0, 0, 0, 0, 1, 0 }));
    EXPECT_EQ(board_.playerPrestige(1), Gems({ 0, 0, 1, 1, 1, 0 }));
    EXPECT_EQ(board_.playerPrestige(2), Gems({ 0, 0, 0, 0, 2, 0 }));
}

TEST_F(MidGameBoard, correctPoints)
{
    EXPECT_EQ(board_.playerPoints(0), 1);
    EXPECT_EQ(board_.playerPoints(1), 0);
    EXPECT_EQ(board_.playerPoints(2), 0);
}

TEST_F(MidGameBoard, correctRemainingDecks)
{
    EXPECT_EQ(board_.remainingCards(LOW), deckCount(g_deck, LOW) - (4 + 7));
    EXPECT_EQ(board_.remainingCards(MEDIUM), deckCount(g_deck, MEDIUM) - (4 + 1));
    EXPECT_EQ(board_.remainingCards(HIGH), deckCount(g_deck, HIGH) - 4);
}

// Buy card, check all is OK
TEST_F(MidGameBoard, normalBuy)
{
}

// Buy card already purchased. By same player; by other player.
TEST_F(MidGameBoard, doubleBuy)
{
}

// Reserve a visible card, check all is OK
TEST_F(MidGameBoard, normalReserve)
{
}

// Reserveundealt deck card, check all is OK
TEST_F(MidGameBoard, hiddenReserve)
{
}

// Buy card already purchased. By same player; by other player. Hidden or not
TEST_F(MidGameBoard, doubleReserve)
{
}

// Buy and reserve when no more cards available to replace.
TEST_F(MidGameBoard, noReplacementCard)
{
}


// Test the board to the point that there are no cards left on the table
TEST_F(MidGameBoard, cardExhaustion)
{
// Should throw.
}
