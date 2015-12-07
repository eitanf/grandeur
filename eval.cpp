// Definitions for evaluation functions and utilities
//
// Created by eitan on 12/5/15.
//

#include <cassert>
#include <algorithm>

#include "eval.h"
#include "move.h"

using namespace std;

namespace grandeur {

template <typename OP>
Scores addScores(const Scores lhs, const Scores& rhs)
{
    assert(lhs.size() == rhs.size());
    Scores ret;
    ret.reserve(lhs.size());
    transform(lhs.begin(), lhs.end(), rhs.begin(), back_inserter(ret), OP());
    return ret;
}

//////////////////////////////////////////////////////////////
Scores operator+(const Scores& lhs, const Scores& rhs)
{
    return addScores<std::plus<score_t>>(lhs, rhs);
}


Scores operator-(const Scores& lhs, const Scores& rhs)
{
    return addScores<std::minus<score_t>>(lhs, rhs);
}


Scores operator*(score_t scalar, const Scores& scores)
{
    Scores ret;
    ret.reserve(scores.size());
    transform(scores.cbegin(), scores.cend(), back_inserter(ret),
              [=](score_t score){ return score * scalar; });
    return ret;
}

//////////////////////////////////////////////////////////////
evaluator_t combine(const std::vector<evaluator_t>& evaluators,
                    const std::vector<score_t>& weights)
{
    assert(evaluators.size() == weights.size());
    return [=](const Moves& moves,
        const player_id_t pid,
        const Board& curBoard,
        const std::vector<Board>& newBoards,
        const Cards& hidden)
    {
        Scores sums(moves.size(), 0);
        for (size_t i = 0; i < evaluators.size(); ++i) {
            sums = sums + weights[i] * evaluators[i](moves, pid, curBoard, newBoards, hidden);
        }
        return sums;
    };
}


//////////////////////////////////////////////////////////////
Scores computeScores(const evaluator_t evaluator, const Moves& moves,
                     const player_id_t pid, const Board& curBoard,
                     const Cards& hidden)
{
    vector<Board> newBoards;
    newBoards.reserve(moves.size());
    auto myhidden = hidden;
    for (const auto& mv : moves) {
        Board board(curBoard);
        if (mv.type_ != MoveType::RESERVE_CARD || !mv.payload_.card_.isWild()) {
            makeMove(board, pid, mv, myhidden, NULL_CARD);
        }
        newBoards.push_back(board);
    }

    return evaluator(moves, pid, curBoard, newBoards, hidden);
}


//////////////////////////////////////////////////////////////
/// Evaluation functions
//////////////////////////////////////////////////////////////

Scores countPoints(const Moves& moves, const player_id_t pid, const Board& board,
                   const std::vector<Board>& newBoards, const Cards&)
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
Scores countPrestige(const Moves& moves, const player_id_t pid, const Board& board,
                     const std::vector<Board>& newBoards, const Cards&)
{
    Scores ret;
    ret.reserve(newBoards.size());

    transform(moves.cbegin(), moves.cend(), back_inserter(ret),
              [=](const GameMove& mv){ return (mv.type_ == MoveType::BUY_CARD)? 1 : 0; });
    return ret;
}


//////////////////////////////////////////////////////////////
Scores winCondition(const Moves& moves, const player_id_t pid, const Board& curBoard,
                    const std::vector<Board>& newBoards, const Cards& hidden)
{

    Scores ret;
    ret.reserve(newBoards.size());
    transform(newBoards.cbegin(), newBoards.cend(), back_inserter(ret),
              [=](const Board& board) {
                  return (board.gameOver() && board.leadingPlayer() == pid)? 1 : 0;
              });
    return ret;
}


} // namespace