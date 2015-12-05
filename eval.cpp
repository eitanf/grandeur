// Definitions for evaluation functions and utilities
//
// Created by eitan on 12/5/15.
//

#include <cassert>
#include <algorithm>

#include "eval.h"

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

//////////////////////////////////////////////////////////////
evaluator_t operator+(const evaluator_t& lhs, const evaluator_t& rhs)
{
    return ([=](const Moves& moves,
                const player_id_t pid,
                const Board& curBoard,
                std::vector<Board>& newBoards,
                const Cards& hidden) {
        return lhs(moves, pid, curBoard, newBoards, hidden) +
               rhs(moves, pid, curBoard, newBoards, hidden);
    });
}


//////////////////////////////////////////////////////////////
/// Evaluation functions
//////////////////////////////////////////////////////////////

Scores countPoints(const Moves& moves, const player_id_t pid, const Board& board,
                   std::vector<Board>& newBoards, const Cards&)
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

} // namespace