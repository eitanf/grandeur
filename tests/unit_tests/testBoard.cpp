//
// Unit tests for Board class
// Created by eitan on 11/19/15.
//


#include <algorithm>
#include <vector>

#include "gtest/gtest.h"

#include "board.h"
#include "move.h"

using namespace grandeur;
using namespace std;

// Utility functions:
// Count the no. of moves of each type in a collection of moves:
unsigned take2MovesNum(const vector<GameMove>& moves)
{
    return count_if(moves.cbegin(), moves.cend(), [](const GameMove& mv) {
        return (mv.type_ == MoveType::TAKE_GEMS
                && mv.payload_.gems_.positiveColors() == 1) ? 1 : 0;
    });
}
unsigned take3MovesNum(const vector<GameMove>& moves)
{
    return count_if(moves.cbegin(), moves.cend(), [](const GameMove& mv) {
        return (mv.type_ == MoveType::TAKE_GEMS
                && mv.payload_.gems_.positiveColors() == 3) ? 1 : 0;
    });
}
unsigned buyMovesNum(const vector<GameMove>& moves)
{
    return count_if(moves.cbegin(), moves.cend(), [](const GameMove& mv) {
        return (mv.type_ == MoveType::BUY_CARD)? 1 : 0;
    });
}
unsigned reserveMovesNum(const vector<GameMove>& moves)
{
    return count_if(moves.cbegin(), moves.cend(), [](const GameMove& mv) {
        return (mv.type_ == MoveType::RESERVE_CARD)? 1 : 0;
    });
}


TEST(boardTests, ctor)
{
    Cards cards({ g_deck[0], g_deck[1], g_deck[2], g_deck[3],
                  g_deck[40], g_deck[41], g_deck[42], g_deck[43],
                  g_deck[70], g_deck[71], g_deck[72], g_deck[73] });
    Board board(2, cards, { g_nobles[5], g_nobles[6], g_nobles[7], g_nobles[8] });

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

#define TAKE(p, gems) EXPECT_EQ(LEGAL_MOVE, board_.takeGems(p, gems))
#define BUY(p, card, rep) EXPECT_EQ(LEGAL_MOVE, board_.buyCard(p, card.id_, hidden_[p], rep))
#define RESERVE(p, card, rep) EXPECT_EQ(LEGAL_MOVE, board_.reserveCard(p, card, hidden_[p],  rep))

class MidGameBoard : public ::testing::Test {
  public:
    MidGameBoard();

    static constexpr unsigned nplayer_ = 3;
    Cards initial_;
    Board::Nobles nobles_;
    Board board_;
    Cards hidden_[nplayer_];
};

MidGameBoard::MidGameBoard() : initial_({g_deck[0], g_deck[3], g_deck[4], g_deck[6],
                                         g_deck[40], g_deck[41], g_deck[42], g_deck[43],
                                         g_deck[70], g_deck[71], g_deck[72], g_deck[73] }),
                               nobles_({ g_nobles[5], g_nobles[6], g_nobles[7], g_nobles[8] }),
                               board_(nplayer_, initial_, nobles_),
                               hidden_()
{
    // Make some game moves (6 rounds):
    TAKE(0, Gems({ 0, 2, 0, 0, 0 }));
    TAKE(1, Gems({ 1, 1, 1, 0, 0 }));
    TAKE(2, Gems({ 0, 0, 0, 2, 0 }));

    TAKE(0, Gems({ 1, 1, 0, 1, 0 }));
    TAKE(1, Gems({ 1, 1, 0, 1, 0 }));
    TAKE(2, Gems({ 0, 0, 1, 1, 1 }));

    RESERVE(0, g_deck[7], NULL_CARD);  // Reserve from LOW deck
    BUY(1, g_deck[0], g_deck[12]);
    BUY(2, g_deck[3], g_deck[24]);

    BUY(0, g_deck[7], NULL_CARD);
    TAKE(1, Gems({ 1, 1, 0, 1, 0 }));
    TAKE(2, Gems({ 0, 1, 1, 1, 0 }));

    RESERVE(0, g_deck[42], NULL_CARD);  // Reserve from MEDIUM table cards, no replacement
    BUY(1, g_deck[24], g_deck[32]);
    TAKE(2, Gems({ 1, 1, 0, 1, 0 }));
    RESERVE(0, g_deck[44], NULL_CARD); // Reserve from MEDIUM deck
    BUY(1, g_deck[32], g_deck[1]);
    BUY(2, g_deck[1], NULL_CARD);
 }


// Verify that all accessors return the expected values
TEST_F(MidGameBoard, correctTableCards)
{
    auto& tcards = board_.tableCards();
    EXPECT_EQ(tcards.size(), initial_.size() - 2);
    EXPECT_EQ(deckCount(LOW, tcards), 3);
    EXPECT_EQ(deckCount(MEDIUM, tcards), 3);
    EXPECT_EQ(deckCount(HIGH, tcards), 4);
}


TEST_F(MidGameBoard, correctReserves)
{
    EXPECT_EQ(board_.playerReserves(0).size(), 1);
    EXPECT_EQ(board_.playerReserves(0).at(0).id_, CardID({ MEDIUM, 42 }));
    EXPECT_EQ(board_.playerReserves(1).size(), 0);
    EXPECT_EQ(board_.playerReserves(2).size(), 0);

    EXPECT_EQ(hidden_[0].size(), 1);
    EXPECT_EQ(hidden_[0].at(0).id_, CardID({ MEDIUM, 44 }));
    EXPECT_EQ(hidden_[1].size(), 0);
    EXPECT_EQ(hidden_[2].size(), 0);
}


TEST_F(MidGameBoard, correctGems)
{
    EXPECT_EQ(board_.tableGems(),   Gems({ 4, 5, 5, 3, 5, 3 }));
    EXPECT_EQ(board_.playerGems(0), Gems({ 1, 0, 0, 1, 0, 2 }));
    EXPECT_EQ(board_.playerGems(1), Gems({ 0, 0, 0, 0, 0, 0 }));
    EXPECT_EQ(board_.playerGems(2), Gems({ 0, 0, 0, 1, 0, 0 }));
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
    // We bought 4 LOW cards w/replacement, and reserved one w/replacment
    EXPECT_EQ(board_.remainingCards(LOW), deckCount(LOW, g_deck) - (4 + 4 + 1));
    // Reserved one MEDIUM card from the deck
    EXPECT_EQ(board_.remainingCards(MEDIUM), deckCount(MEDIUM, g_deck) - (4 + 1));
    EXPECT_EQ(board_.remainingCards(HIGH), deckCount(HIGH, g_deck) - 4);
}


// Check that all (and only) the legal moves show up with legalMoves()
TEST_F(MidGameBoard, legalMoves)
{
  // Make things a bit more interesting first:
  TAKE(1, Gems({ 2 }));
  TAKE(1, Gems({ 0, 2 }));
  TAKE(1, Gems({ 1, 1, 1 }));
  TAKE(1, Gems({ 1, 1, 1 }));
  RESERVE(0, g_deck[8], NULL_CARD);
  RESERVE(2, g_deck[12], g_deck[14]);

  const auto p0moves = legalMoves(board_, 0, hidden_[0]);
  const auto p1moves = legalMoves(board_, 1, hidden_[1]);
  const auto p2moves = legalMoves(board_, 2, hidden_[2]);

  EXPECT_EQ(take2MovesNum(p0moves), 1);
  EXPECT_EQ(take2MovesNum(p1moves), 0);
  EXPECT_EQ(take2MovesNum(p2moves), 1);

  EXPECT_EQ(take3MovesNum(p0moves), 4);
  EXPECT_EQ(take3MovesNum(p1moves), 0);
  EXPECT_EQ(take3MovesNum(p2moves), 4);

  EXPECT_EQ(buyMovesNum(p0moves), 4);
  EXPECT_EQ(buyMovesNum(p1moves), 3);
  EXPECT_EQ(buyMovesNum(p2moves), 2);

  EXPECT_EQ(reserveMovesNum(p0moves), 0);  // Too many reserves
  EXPECT_EQ(reserveMovesNum(p1moves), 0);  // Too many gems
  EXPECT_EQ(reserveMovesNum(p2moves), 13);
}


// Test all sorts of legal and illegal gem takes:
TEST_F(MidGameBoard, testTakeGems)
{
    EXPECT_EQ(board_.takeGems(0, Gems({ 0, 0, 0, 1, 1, 1 })), TAKING_YELLOW);
    EXPECT_EQ(board_.takeGems(0, Gems({ 0, 0, 0, 0, 0, 2 })), TAKING_YELLOW);

    TAKE(0, Gems({ 1, 1, 1 }));
    TAKE(0, Gems({ 0, 2, 0 }));

    EXPECT_EQ(board_.takeGems(0, Gems({ 0, 0, 0, 1, -1, -1 })), INSUFFICIENT_GEMS_TO_RETURN);
    EXPECT_EQ(board_.takeGems(0, Gems({ 1, 1, 1 })), TOO_MANY_GEMS); // We have 9 gems

    TAKE(0, Gems({ 0, 0, 1, 1, 0, -1 }));  // Now 10
    EXPECT_EQ(board_.takeGems(0, Gems({ 0, 0, 1, 1, 0, -1 })), TOO_MANY_GEMS);
    EXPECT_EQ(board_.takeGems(0, Gems({ 0, 0, -1, 1, 0, -1 })), WRONG_NUMBER_OF_GEMS);
    EXPECT_EQ(board_.takeGems(0, Gems({ 0, 0, -1, -1, 1, 0 })), WRONG_NUMBER_OF_GEMS);

    BUY(0, g_deck[4], NULL_CARD);
    RESERVE(0, g_deck[73], NULL_CARD); // Back to 8 gems

    EXPECT_EQ(board_.takeGems(0, Gems({ 2, 0, 0 })), INSUFFICIENT_TABLE_GEMS);
    EXPECT_EQ(board_.takeGems(0, Gems({ 1, 0, 0 })), WRONG_NUMBER_OF_GEMS);
    EXPECT_EQ(board_.takeGems(0, Gems({ 0, 1, -1, -1, 1 })), INSUFFICIENT_GEMS_TO_RETURN);
    EXPECT_EQ(board_.takeGems(0, Gems({ -1, -1, 1, 0, 1 })), WRONG_NUMBER_OF_GEMS);
    EXPECT_EQ(board_.takeGems(0, Gems({ 1, 1, 2 })), TOO_MANY_GEMS);
    EXPECT_EQ(board_.takeGems(0, Gems({ 1, 1, 2, 1 })), TOO_MANY_GEMS);

    EXPECT_EQ(board_.takeGems(0, Gems({ 0, 0, 2, 0 })), LEGAL_MOVE);
}


// Can't end with more than 10.
// If has anything other than 1/2 or 3/1 must end on 10,
//      but must also start above 7, and can't do 1/3 or 2+1
// Ensure all takes and returns are valid, even if ending with 10
TEST_F(MidGameBoard, returnGems)
{
    TAKE(1, Gems({1, 1, 1}));
    TAKE(1, Gems({0, 1, 0, 1, 1}));
    TAKE(1, Gems({0, 1, 0, 1, 1})); // Got to 9 gems

    // Can't just take regular gems now, must return some:
    EXPECT_EQ(board_.takeGems(1, Gems({ 1, 1, 1, 0, 0, 0 })), TOO_MANY_GEMS);
    EXPECT_EQ(board_.takeGems(1, Gems({ 0, 0, 2, 0, 0, 0 })), TOO_MANY_GEMS);

    EXPECT_EQ(board_.takeGems(1, Gems({ -1, 0, 2, 0, 0, 0 })), LEGAL_MOVE); // OK: we're at 10
    EXPECT_EQ(board_.takeGems(1, Gems({ 2, 0, -2, 0, 0, 0 })), LEGAL_MOVE); // OK: we're at 10

    EXPECT_EQ(board_.takeGems(1, Gems({ 0, 1, 1, 1, -1, 0 })), TOO_MANY_GEMS);
    EXPECT_EQ(board_.takeGems(1, Gems({ -3, 0, 2, 0, 0, 0 })), INSUFFICIENT_GEMS_TO_RETURN);
    EXPECT_EQ(board_.takeGems(1, Gems({ 0, -3, 2, 0, 0, 0 })), WRONG_NUMBER_OF_GEMS);
    EXPECT_EQ(board_.takeGems(1, Gems({ 0, -2, 2, -1, 0, 0 })), WRONG_NUMBER_OF_GEMS);
    EXPECT_EQ(board_.takeGems(1, Gems({ 1, 1, -1, -1, 0, 0 })), LEGAL_MOVE);
    EXPECT_EQ(board_.takeGems(1, Gems({ 0, -1, 0, 1, 0, 0 })), LEGAL_MOVE);
    EXPECT_EQ(board_.takeGems(1, Gems({ 1, -3, 1, 1, 0, 0 })), LEGAL_MOVE);
    EXPECT_EQ(board_.takeGems(1, Gems({ 0, 0, 0, 0, 0, 0 })), LEGAL_MOVE);

    // Go back to 7 gems:
    BUY(1, g_deck[12], NULL_CARD);
    std::cerr << board_;
    EXPECT_EQ(board_.takeGems(1, Gems({ 1, 1, 1, -1, 1, 0 })), TOO_MANY_GEMS);
    EXPECT_EQ(board_.takeGems(1, Gems({ -2, 2, 0, 0, 0, 0 })), WRONG_NUMBER_OF_GEMS);
    EXPECT_EQ(board_.takeGems(1, Gems({ -2, 2, 2, 0, 0, 0 })), WRONG_NUMBER_OF_GEMS);
    EXPECT_EQ(board_.takeGems(1, Gems({ 1, 1, 1, -3, 0, 0 })), WRONG_NUMBER_OF_GEMS);
    EXPECT_EQ(board_.takeGems(1, Gems({ 0, 0, 0, 0, 0, 0 })), WRONG_NUMBER_OF_GEMS);
}

// Try to buy a card with insufficient gems:
TEST_F(MidGameBoard, insufficientBuy)
{
    RESERVE(1, g_deck[71], NULL_CARD);
    RESERVE(1, g_deck[72], NULL_CARD);
    RESERVE(1, g_deck[73], NULL_CARD);
    TAKE(1, Gems({ 0, 2 }));
    EXPECT_EQ(board_.buyCard(1, g_deck[40].id_, hidden_[1], NULL_CARD), INSUFFICIENT_GEMS);
}


// Try to buy a card already bought:
TEST_F(MidGameBoard, doubleBuy)
{
    TAKE(1, Gems({1, 1, 1}));
    TAKE(1, Gems({0, 1, 0, 1, 1}));
    // A card I already bought:
    EXPECT_EQ(board_.buyCard(1, g_deck[0].id_, hidden_[1], NULL_CARD), UNAVAILABLE_CARD);
    // A card someone else already bought:
    EXPECT_EQ(board_.buyCard(1, g_deck[1].id_, hidden_[1], NULL_CARD), UNAVAILABLE_CARD);
}


// Try to buy a card that's not currently visible on table cards:
TEST_F(MidGameBoard, unavailableBuy)
{
    TAKE(1, Gems({1, 1, 1}));
    TAKE(1, Gems({0, 1, 0, 1, 1}));
    // A card not for sale:
    EXPECT_EQ(board_.buyCard(1, g_deck[16].id_, hidden_[1], NULL_CARD), UNAVAILABLE_CARD);
}


// Try to buy any sort of card that doesn't exist:
TEST_F(MidGameBoard, invalidBuy)
{
    EXPECT_EQ(board_.buyCard(1, { LOW, 58 }, hidden_[1], NULL_CARD), UNAVAILABLE_CARD);
    EXPECT_EQ(board_.buyCard(1, { LOW, -10 }, hidden_[1], NULL_CARD), UNAVAILABLE_CARD);
    EXPECT_EQ(board_.buyCard(1, { LOW, CardID::WILDCARD }, hidden_[1], NULL_CARD), BUY_WILDCARD);
    EXPECT_EQ(board_.buyCard(1, { LOW, CardID::NULLCARD}, hidden_[1], NULL_CARD), UNAVAILABLE_CARD);
}


// Reserve card already purchased or reserved:
TEST_F(MidGameBoard, unavailableReserve)
{
    EXPECT_EQ(board_.reserveCard(0, g_deck[0], hidden_[0], NULL_CARD), UNAVAILABLE_CARD);
    EXPECT_EQ(board_.reserveCard(0, g_deck[42], hidden_[0], NULL_CARD), UNAVAILABLE_CARD);
    EXPECT_EQ(board_.reserveCard(0, g_deck[44], hidden_[0], NULL_CARD), UNAVAILABLE_CARD);
    EXPECT_EQ(board_.reserveCard(2, g_deck[1], hidden_[2], NULL_CARD), UNAVAILABLE_CARD);
    EXPECT_EQ(board_.reserveCard(2, g_deck[42], hidden_[2], NULL_CARD), UNAVAILABLE_CARD);
}


// Exceed reservation maximum cards:
TEST_F(MidGameBoard, tooManyReserves)
{
    RESERVE(1, g_deck[13], NULL_CARD);
    RESERVE(1, g_deck[14], NULL_CARD);
    RESERVE(1, g_deck[15], NULL_CARD);
    EXPECT_EQ(board_.reserveCard(1, g_deck[16], hidden_[1], NULL_CARD), TOO_MANY_RESERVES);
}


// Exceed reservation maximum gems:
TEST_F(MidGameBoard, tooManyGemsToReserve)
{
    TAKE(1, Gems({1, 1, 1}));
    TAKE(1, Gems({0, 1, 0, 1, 1}));
    TAKE(1, Gems({0, 1, 0, 1, 1}));
    RESERVE(1, g_deck[14], NULL_CARD); // 10 gems
    EXPECT_EQ(board_.reserveCard(1, g_deck[15], hidden_[1], NULL_CARD), TOO_MANY_GEMS);
}


// Exhaust table yellows:
TEST_F(MidGameBoard, insufficientGemsToReserve)
{
    ASSERT_EQ(board_.tableGems().getCount(YELLOW), 3);
    RESERVE(1, g_deck[13], NULL_CARD);
    RESERVE(1, g_deck[14], NULL_CARD);
    RESERVE(1, g_deck[15], NULL_CARD);
    EXPECT_EQ(board_.reserveCard(2, g_deck[15], hidden_[2], NULL_CARD), INSUFFICIENT_TABLE_GEMS);
}


// Test the board to the point that there are no cards left on the table
TEST_F(MidGameBoard, cardExhaustion)
{
    // Setup: remove all table cards with no replacement:
    BUY(0, g_deck[12], NULL_CARD);
    RESERVE(1, g_deck[71], NULL_CARD);
    RESERVE(1, g_deck[72], NULL_CARD);
    RESERVE(1, g_deck[73], NULL_CARD);
    TAKE(1, Gems({ 0, 2 }));
    TAKE(1, Gems({ 0, 0, 2 }));
    BUY(1, g_deck[40], NULL_CARD);

    RESERVE(2, g_deck[41], NULL_CARD);
    RESERVE(2, g_deck[43], NULL_CARD);
    RESERVE(2, g_deck[70], NULL_CARD);
    BUY(2, g_deck[4], NULL_CARD);

    RESERVE(0, g_deck[6], NULL_CARD);

    // OK, now we have no more cards left on the table. We can buy reserves but no cards (with no deaths):
    TAKE(0, Gems({ 0, 0, 2 }));
    BUY(0, g_deck[6], NULL_CARD);
    ASSERT_EQ(board_.reserveCard(0, g_deck[19], hidden_[1], g_deck[20]), UNAVAILABLE_CARD);
    ASSERT_EQ(board_.buyCard(0, g_deck[19].id_, hidden_[1], NULL_CARD), UNAVAILABLE_CARD);
}

// Test that acquiring a noble updates points correctly.
TEST_F(MidGameBoard, nobleAcquisition)
{
    ASSERT_EQ(board_.tableNobles().size(), nobles_.size());
    ASSERT_EQ(board_.playerPoints(2), 0);
    ASSERT_EQ(board_.playerPrestige(2).getCount(BLACK), 2);

    // Acquire two more blacks:
    TAKE(2, Gems({ 0, 0, 2 }));
    TAKE(2, Gems({ 1, 0, 1, 1 }));
    BUY(2, g_deck[6], g_deck[19]);
    TAKE(2, Gems({ 0, 0, 2 }));
    BUY(2, g_deck[4], g_deck[20]);

    // Acquire four teals:
    BUY(2, g_deck[12], g_deck[13]);
    TAKE(2, Gems({ 0, 0, 2 }));
    BUY(2, g_deck[13], g_deck[14]);
    BUY(2, g_deck[14], g_deck[11]);
    TAKE(2, Gems({ 0, 0, 2 }));
    TAKE(2, Gems({ 1, 1, 1 }));
    BUY(2, g_deck[11], g_deck[21]);

    // Acquire four whites:
    BUY(2, g_deck[21], g_deck[23]);
    TAKE(2, Gems({ 1, 0, 1, 1, 0 }));
    BUY(2, g_deck[19], NULL_CARD);
    TAKE(2, Gems({ 0, 0, 2 }));
    TAKE(2, Gems({ 1, 0, 1, 1, 0 }));
    BUY(2, g_deck[23], NULL_CARD);
    BUY(2, g_deck[20], NULL_CARD);


    EXPECT_EQ(board_.playerPoints(2), 7);  // Points from getting the nobles 5 and 8
    ASSERT_EQ(board_.tableNobles().size(), nobles_.size() - 2);
}
