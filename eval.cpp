// Definitions for evaluation functions and utilities
//
// Created by eitan on 12/5/15.
//

#include <algorithm>
#include <cassert>
#include <cmath>

#include "eval.h"
#include "move.h"

using namespace std;

namespace grandeur {

template <typename OP>
Scores
addScores(const Scores lhs, const Scores& rhs)
{
    assert(lhs.size() == rhs.size());
    Scores ret;
    ret.reserve(lhs.size());
    transform(lhs.begin(), lhs.end(), rhs.begin(), back_inserter(ret), OP());
    return ret;
}

//////////////////////////////////////////////////////////////
Scores
operator+(const Scores& lhs, const Scores& rhs)
{
    return addScores<std::plus<score_t>>(lhs, rhs);
}


Scores
operator-(const Scores& lhs, const Scores& rhs)
{
    return addScores<std::minus<score_t>>(lhs, rhs);
}


Scores
operator*(score_t scalar, const Scores& scores)
{
    Scores ret;
    ret.reserve(scores.size());
    transform(scores.cbegin(), scores.cend(), back_inserter(ret),
              [=](score_t score){ return score * scalar; });
    return ret;
}

//////////////////////////////////////////////////////////////
evaluator_t
combine(const std::vector<evaluator_t>& evaluators, const std::vector<score_t>& weights)
{
    assert(evaluators.size() == weights.size());
    return [=](const Moves& moves,
        const player_id_t pid,
        const Board& curBoard,
        const std::vector<Board>& newBoards)
    {
        Scores sums(moves.size(), 0);
        for (size_t i = 0; i < evaluators.size(); ++i) {
            sums = sums + weights[i] * evaluators[i](moves, pid, curBoard, newBoards);
        }
        return sums;
    };
}


//////////////////////////////////////////////////////////////
Scores
computeScores(const evaluator_t evaluator, const Moves& moves, const player_id_t pid,
              const Board& curBoard, vector<Board>& newBoards)
{
    newBoards.clear();

    for (const auto& mv : moves) {
        newBoards.push_back(curBoard);
        const auto status = makeMove(newBoards.back(), pid, mv, NULL_CARD);
        assert(LEGAL_MOVE == status);
    }

    return evaluator(moves, pid, curBoard, newBoards);
}


//////////////////////////////////////////////////////////////
/// Evaluation functions
//////////////////////////////////////////////////////////////

Scores
countPoints(const Moves& moves, const player_id_t pid, const Board& board,
            const std::vector<Board>& newBoards)
{
    Scores ret;
    assert(moves.size() == newBoards.size());
    ret.reserve(newBoards.size());
    const auto pts = board.playerPoints(pid);

    transform(newBoards.cbegin(), newBoards.cend(), back_inserter(ret),
              [=](const Board& board){ return board.playerPoints(pid) - pts; });
    return ret;
}


//////////////////////////////////////////////////////////////
Scores
countPrestige(const Moves& moves, const player_id_t pid, const Board& board,
              const std::vector<Board>& newBoards)
{
    Scores ret;
    ret.reserve(newBoards.size());
    transform(moves.cbegin(), moves.cend(), back_inserter(ret),
              [=](const GameMove& mv){ return (mv.type_ == MoveType::BUY_CARD)? 1 : 0; });
    return ret;
}


//////////////////////////////////////////////////////////////
Scores
winCondition(const Moves& moves, const player_id_t pid, const Board& curBoard,
             const std::vector<Board>& newBoards)
{
    Scores ret;
    ret.reserve(newBoards.size());
    transform(newBoards.cbegin(), newBoards.cend(), back_inserter(ret),
              [=](const Board& board) {
                  return (board.gameOver() && board.leadingPlayer() == pid)? 1 : 0;
              });
    return ret;
}


//////////////////////////////////////////////////////////////
Scores
countGems(const Moves& moves, const player_id_t pid, const Board& curBoard,
          const std::vector<Board>& newBoards)
{
    const auto curGems = curBoard.playerGems(pid).totalGems();
    Scores ret;
    ret.reserve(newBoards.size());
    transform(newBoards.cbegin(), newBoards.cend(), back_inserter(ret),
              [=](const Board& board)
              {
                  return score_t(board.playerGems(pid).totalGems() - curGems)
                         / DIFFERENT_COLOR_GEMS;
              });
    return ret;
}


//////////////////////////////////////////////////////////////
Scores
countMoves(const Moves& moves, const player_id_t pid, const Board& curBoard,
           const std::vector<Board>& newBoards)
{
    return Scores(moves.size(), score_t(moves.size()) / MEAN_MOVES);
}


//////////////////////////////////////////////////////////////
static score_t
gemNumOfColor(const Cards& cards, gem_color_t color)
{
    return count_if(cards.cbegin(), cards.cend(), [=](const Card& card)
    {
        return card.color_ == color;
    });
}


Scores
monopolizeGems(const Moves& moves, const player_id_t pid, const Board& curBoard,
               const std::vector<Board>& newBoards)
{
    const auto nCards = curBoard.tableCards().size();
    Scores ret;
    ret.reserve(newBoards.size());
    const score_t colorCount[] = {
            gemNumOfColor(curBoard.tableCards(), WHITE),
            gemNumOfColor(curBoard.tableCards(), TEAL),
            gemNumOfColor(curBoard.tableCards(), GREEN),
            gemNumOfColor(curBoard.tableCards(), RED),
            gemNumOfColor(curBoard.tableCards(), BLACK)
    };

    transform(moves.cbegin(), moves.cend(), back_inserter(ret),
              [=](const GameMove& mv)
              {
                  return (mv.type_ != MoveType::TAKE_GEMS
                       && !mv.payload_.card_.isWild())?
                         1. - colorCount[mv.payload_.card_.color_] / nCards
                       : 0;
              });
    return ret;
}


//////////////////////////////////////////////////////////////
Scores
preferWildcards(const Moves& moves, const player_id_t pid, const Board& curBoard,
                const std::vector<Board>& newBoards)
{
    Scores ret;
    ret.reserve(newBoards.size());
    transform(moves.cbegin(), moves.cend(), back_inserter(ret),
              [=](const GameMove& mv)
              {
                  return (mv.type_ == MoveType::RESERVE_CARD
                       && mv.payload_.card_.isWild())? 1 : 0.;
              });
    return ret;
}


//////////////////////////////////////////////////////////////
Scores
countReturns(const Moves& moves, const player_id_t pid, const Board& curBoard,
             const std::vector<Board>& newBoards)
{
    Scores ret;
    ret.reserve(newBoards.size());
    transform(moves.cbegin(), moves.cend(), back_inserter(ret),
              [=](const GameMove& mv)
              {
                  return (mv.type_ == MoveType::TAKE_GEMS
                       && mv.payload_.gems_.hasNegatives())? 1 : 0;
              });
    return ret;
}


//////////////////////////////////////////////////////////////
Scores
preferShortGame(const Moves& moves, const player_id_t pid, const Board& curBoard,
                const std::vector<Board>& newBoards)
{
    return Scores(moves.size(),
                  std::log(1. + curBoard.roundNumber()) / std::log(1. / MAX_GAME_ROUNDS));
}


//////////////////////////////////////////////////////////////
Scores
preferBuyTowardNoble(const Moves& moves, const player_id_t pid, const Board& curBoard,
                     const std::vector<Board>& newBoards)
{
    Scores ret(moves.size(), 0.);
    if (curBoard.tableNobles().empty()) {
        return ret;
    }

    for (unsigned i = 0; i < moves.size(); ++i) {
        if (moves[i].type_ == BUY_CARD) {
            const auto color = moves[i].payload_.card_.color_;
            for (const auto& n : curBoard.tableNobles()) {
                const auto nCost = n.cost_.getCount(color);
                if (nCost > 0
                 && nCost > curBoard.playerPrestige(pid).getCount(color)) {
                    ++ret[i];
                }
            }
        }
    }
    return (1. / curBoard.tableNobles().size()) * ret;
}

} // namespace