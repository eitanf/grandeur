//
// Unit tests for Board class
// Created by eitan on 11/19/15.
//


#include "gtest/gtest.h"
#include "board.h"

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

    EXPECT_EQ(board.remainingCards(LOW), deckCount(LOW, g_deck) - 4);
    EXPECT_EQ(board.remainingCards(MEDIUM), deckCount(MEDIUM, g_deck) - 4);
    EXPECT_EQ(board.remainingCards(HIGH), deckCount(HIGH, g_deck) - 4);
}

#define TAKE(p, gems) EXPECT_EQ(LEGAL_MOVE,                                 \
        board_.makeMove(p, GameMove(gems), hidden_[p]))
#define BUY(p, card, rep) EXPECT_EQ(LEGAL_MOVE,                              \
        board_.makeMove(p, GameMove(card, MoveType::BUY_CARD), hidden_[p], rep))
#define RESERVE(p, card, rep) EXPECT_EQ(LEGAL_MOVE,                          \
        board_.makeMove(p, GameMove(card, MoveType::RESERVE_CARD), hidden_[p],  rep))

class MidGameBoard : public ::testing::Test {
  public:
    MidGameBoard()
       : initial_({ g_deck[0], g_deck[3], g_deck[4], g_deck[6],
                    g_deck[40], g_deck[41], g_deck[42], g_deck[43],
                    g_deck[70], g_deck[71], g_deck[72], g_deck[73] }),
         board_(nplayer_, initial_),
         hidden_()
    {
        // Make some game moves (6 rounds):
        TAKE(0, Gems({ 0, 2, 0, 0, 0 }));
        TAKE(1, Gems({ 1, 1, 1, 0, 0 }));
        TAKE(2, Gems({ 0, 0, 0, 2, 0 }));

        TAKE(0, Gems({ 1, 1, 0, 1, 0 }));
        TAKE(1, Gems({ 1, 1, 0, 1, 0 }));
        TAKE(2, Gems({ 0, 0, 1, 1, 1 }));

        RESERVE(0, g_deck[7], NULL_CARD);
        BUY(1, g_deck[0], g_deck[12]);
        BUY(2, g_deck[3], g_deck[24]);

        BUY(0, g_deck[7], NULL_CARD);
        TAKE(1, Gems({ 1, 1, 0, 1, 0 }));
        TAKE(2, Gems({ 0, 1, 1, 1, 0 }));

        RESERVE(0, g_deck[42], NULL_CARD);  // MEDIUM
        BUY(1, g_deck[24], g_deck[32]);
        TAKE(2, Gems({ 1, 1, 0, 1, 0 }));
        RESERVE(0, g_deck[6], g_deck[5]);
        BUY(1, g_deck[32], g_deck[1]);
        BUY(2, g_deck[1], NULL_CARD);
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
    EXPECT_EQ(deckCount(LOW, tcards), 3);
    EXPECT_EQ(deckCount(MEDIUM, tcards), 4);
    EXPECT_EQ(deckCount(HIGH, tcards), 4);
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
    EXPECT_EQ(board_.remainingCards(LOW), deckCount(LOW, g_deck) - (4 + 7));
    EXPECT_EQ(board_.remainingCards(MEDIUM), deckCount(MEDIUM, g_deck) - (4 + 1));
    EXPECT_EQ(board_.remainingCards(HIGH), deckCount(HIGH, g_deck) - 4);
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
