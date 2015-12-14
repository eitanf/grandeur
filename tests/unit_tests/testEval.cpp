// Test misc evaluation functions
//
// Created by eitan on 12/13/15.
//

#include "gtest/gtest.h"

#include "board.h"
#include "eval.h"
#include "move.h"

using namespace grandeur;
using namespace std;

#define TAKE(p, gems) EXPECT_EQ(LEGAL_MOVE, board_.takeGems(p, gems))
#define BUY(p, card, rep) EXPECT_EQ(LEGAL_MOVE, board_.buyCard(p, card.id_, rep))
#define RESERVE(p, card, rep) EXPECT_EQ(LEGAL_MOVE, board_.reserveCard(p, card,  rep))

// utility: find the score of a specific move:
score_t scoreOf(const Scores& scores, const Moves& legal, GameMove m)
{
    const auto iter = find(legal.cbegin(), legal.end(), m);
    EXPECT_NE(iter, legal.cend());
    return scores.at(distance(legal.cbegin(), iter));
}


class LateGameBoard : public ::testing::Test {
  public:
    LateGameBoard();
    Scores playerScores(player_id_t pid, evaluator_t eval) const;

    static constexpr unsigned nplayer_ = 3;
    Cards initial_;
    Board::Nobles nobles_;
    Board board_;
    Moves moves1_; // Moves for player 1
};

LateGameBoard::LateGameBoard() : initial_({g_deck[0], g_deck[3], g_deck[4], g_deck[6],
                                           g_deck[40], g_deck[41], g_deck[42], g_deck[43],
                                           g_deck[70], g_deck[71], g_deck[72], g_deck[73] }),
                                 nobles_({ g_nobles[5], g_nobles[6], g_nobles[7], g_nobles[8] }),
                                 board_(nplayer_, initial_, nobles_)
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

    // Start buying up cards for player 1:
    RESERVE(1, g_deck[54], NULL_CARD);
    TAKE(1, Gems({ 0, 1, 1, 1, 0 }));
    TAKE(1, Gems({ 1, 0, 0, 1, 1 }));
    BUY(1, g_deck[54], NULL_CARD);
    RESERVE(1, g_deck[33], NULL_CARD);
    BUY(1, g_deck[33], NULL_CARD);
    RESERVE(1, g_deck[41], NULL_CARD);
    TAKE(1, Gems({ 1, 0, 1, 0, 1 }));
    BUY(1, g_deck[41], NULL_CARD);
    RESERVE(1, g_deck[46], NULL_CARD);
    TAKE(1, Gems({ 0, 1, 1, 1, 0 }));
    BUY(1, g_deck[46], NULL_CARD);
    RESERVE(1, g_deck[10], NULL_CARD);
    BUY(1, g_deck[10], NULL_CARD);
    RESERVE(1, g_deck[17], NULL_CARD);
    BUY(1, g_deck[17], NULL_CARD);
    RESERVE(1, g_deck[28], NULL_CARD);
    BUY(1, g_deck[28], NULL_CARD);
    RESERVE(1, g_deck[53], NULL_CARD);
    BUY(1, g_deck[53], NULL_CARD);
    RESERVE(1, g_deck[58], NULL_CARD);
    BUY(1, g_deck[58], NULL_CARD);
    RESERVE(1, g_deck[60], NULL_CARD);
    BUY(1, g_deck[60], NULL_CARD);
    RESERVE(1, g_deck[62], NULL_CARD);
    BUY(1, g_deck[62], NULL_CARD);
    RESERVE(1, g_deck[63], NULL_CARD);
    BUY(1, g_deck[63], NULL_CARD);
    RESERVE(1, g_deck[65], NULL_CARD);
    BUY(1, g_deck[65], NULL_CARD);
    // Player one on the verge of winning now with 14 points

    moves1_ = legalMoves(board_, 1);
}


Scores
LateGameBoard::playerScores(player_id_t pid, evaluator_t eval) const
{
    vector<Board> dummy;
    const auto moves = legalMoves(board_, pid);
    const auto scores = computeScores(eval, moves, pid, board_, dummy);
    return scores;
}


/////////////////////////////////////////////////////////////////////////
TEST_F(LateGameBoard, countPoints)
{
    auto scores = playerScores(0, countPoints);
    EXPECT_TRUE(all_of(scores.cbegin(), scores.cend(), [](auto s){ return s == 0; }));

    scores = playerScores(2, countPoints);
    EXPECT_TRUE(all_of(scores.cbegin(), scores.cend(), [](auto s){ return s == 0; }));

    scores = playerScores(1, countPoints);
    EXPECT_EQ(0, scoreOf(scores, moves1_, { g_deck[12], BUY_CARD } ));
    EXPECT_EQ(0, scoreOf(scores, moves1_, { g_deck[04], BUY_CARD } ));
    EXPECT_EQ(0, scoreOf(scores, moves1_, { g_deck[06], BUY_CARD } ));
    EXPECT_EQ(1, scoreOf(scores, moves1_, { g_deck[40], BUY_CARD } ));
    EXPECT_EQ(2, scoreOf(scores, moves1_, { g_deck[43], BUY_CARD } ));
}


TEST_F(LateGameBoard, countPrestige)
{

    const auto scores = playerScores(1, countPrestige);
    for (unsigned i = 0; i < moves1_.size(); ++i) {
        EXPECT_EQ(scores[i], moves1_[i].type_ == BUY_CARD? 1 : 0);
    }
}


TEST_F(LateGameBoard, winCondition)
{
    const auto scores = playerScores(1, winCondition);

    for (unsigned i = 0; i < moves1_.size(); ++i) {
        const auto mv = moves1_[i];
        if (mv == GameMove(g_deck[40], BUY_CARD) || mv  == GameMove(g_deck[43], BUY_CARD)) {
            EXPECT_EQ(scores[i], 1);
        } else {
            EXPECT_EQ(scores[i], 0);
        }
    }
}


TEST_F(LateGameBoard, countGems)
{
    for (player_id_t pid = 0; pid < nplayer_; ++pid) {
        const score_t curGems = board_.playerGems(pid).totalGems();
        const auto moves = legalMoves(board_, pid);
        const auto scores = playerScores(pid, countGems);

        for (unsigned i = 0; i < moves.size(); ++i) {
            const auto mv = moves[i];
            score_t newGems = 0;
            switch (mv.type_) {
            case TAKE_GEMS:
                newGems = curGems + mv.payload_.gems_.totalGems();
                break;
            case RESERVE_CARD:
                newGems = curGems + 1;
                break;
            case BUY_CARD:
                newGems = (curGems - board_.playerGems(pid).actualCost(
                            mv.payload_.card_.cost_ - board_.playerPrestige(pid))).totalGems();
                break;
            }

            EXPECT_EQ(scores[i], (newGems - curGems) / DIFFERENT_COLOR_GEMS);
        }
    }
}


TEST_F(LateGameBoard, countMoves)
{
    for (player_id_t pid = 0; pid < nplayer_; ++pid) {
        const auto moves = legalMoves(board_, pid);
        const auto scores = playerScores(pid, countMoves);
        EXPECT_TRUE(all_of(scores.cbegin(), scores.cend(), [=](auto s)
        {
            return s == (score_t)moves.size() / MEAN_MOVES;
        }));
    }
}


TEST_F(LateGameBoard, monopolizeGems)
{
    const auto scores = playerScores(1, monopolizeGems);
    const auto nCards = board_.tableCards().size();
    EXPECT_EQ(1. - 1. / nCards, scoreOf(scores, moves1_, GameMove(g_deck[12], BUY_CARD)));
    EXPECT_EQ(1. - 1. / nCards, scoreOf(scores, moves1_, GameMove(g_deck[12], RESERVE_CARD)));
    EXPECT_EQ(1. - 8. / nCards, scoreOf(scores, moves1_, GameMove(g_deck[4], BUY_CARD)));
    EXPECT_EQ(1. - 8. / nCards, scoreOf(scores, moves1_, GameMove(g_deck[4], RESERVE_CARD)));
    EXPECT_EQ(1. - 8. / nCards, scoreOf(scores, moves1_, GameMove(g_deck[6], BUY_CARD)));
    EXPECT_EQ(1. - 8. / nCards, scoreOf(scores, moves1_, GameMove(g_deck[6], RESERVE_CARD)));
    EXPECT_EQ(0., scoreOf(scores, moves1_, GameMove(Gems({ 1, 1, 1, 0, 0 }))));
}


TEST_F(LateGameBoard, preferWildcards)
{
    for (player_id_t pid = 0; pid < nplayer_; ++pid) {
        const auto moves = legalMoves(board_, pid);
        const auto scores = playerScores(pid, preferWildcards);

        for (unsigned i = 0; i < moves.size(); ++i) {
            const auto mv = moves[i];
            if (mv.type_ == MoveType::RESERVE_CARD && mv.payload_.card_.isWild()) {
                EXPECT_FLOAT_EQ(scores[i], 1);
            } else {
                EXPECT_FLOAT_EQ(scores[i], 0);
            }
        }
    }
}


TEST_F(LateGameBoard, countReturns)
{
    TAKE(0, Gems({ 2, 0, 0, 0, 0 }));
    TAKE(0, Gems({ 0, 2, 0, 0, 0 }));
    const auto moves = legalMoves(board_, 0);
    const auto scores = playerScores(0, countReturns);

    for (unsigned i = 0; i < moves.size(); ++i) {
        const auto mv = moves[i];
        if (mv.type_ == MoveType::TAKE_GEMS && mv.payload_.gems_.hasNegatives()) {
            EXPECT_FLOAT_EQ(scores[i], 1);
        } else {
            EXPECT_FLOAT_EQ(scores[i], 0);
        }
    }
}

TEST_F(LateGameBoard, preferShortGame)
{

    const auto s0 = playerScores(0, preferShortGame)[0];
    board_.newRound();
    const auto s1 = playerScores(0, preferShortGame)[0];
    EXPECT_LT(s1, s0);
}