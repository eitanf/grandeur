// Declarations for evaluation functions that evaluate a list of potential
// moves and give a score to each move (higher score imply betters move).
// Evaluation functions are meant to be additive, so that each move can be
// evaluated by multiple functions, with the result summed up.
//
// Created by eitan on 12/5/15.
//

#pragma once

#include <functional>
#include <vector>

#include "board.h"
#include "card.h"
#include "move.h"

namespace grandeur {

using score_t = double;

using Scores = std::vector<score_t>;

Scores operator+(const Scores& lhs, const Scores& rhs);
Scores operator-(const Scores& lhs, const Scores& rhs);
Scores operator*(score_t scalar, const Scores& scores);
inline Scores operator*(const Scores& scores, score_t scalar)
{
    return scalar * scores;
}

///// Interface for an evaluation function.
// Receives the moves to be scored, the player id making the move, the
// board state before making the move, and the board states after making each move.
using evaluator_t = std::function<Scores(const Moves& moves,
                                         const player_id_t pid,
                                         const Board& curBoard,
                                         const std::vector<Board>& newBoards)>;


// Combine multiple evaluators to one evaluator, each with its own weight.
// The combined evaluator sums all the weighted evaluators.
evaluator_t combine(const std::vector<evaluator_t>& evaluators,
                    const std::vector<score_t>& weights);


// Compute scores for a given board state and evaluation function
// It'll automatically generate and return the new boards and new legal moves for
// each element of moves.
Scores computeScores(const evaluator_t eval, const Moves& moves, const player_id_t pid, const Board& curBoard,
                     std::vector<Board>& newBoards);


////////////////////////////////////////////////////////////////////
////////// Declarations for various evaluation functions:

// Sum up the new points gained by each move
Scores countPoints(const Moves& moves, const player_id_t pid, const Board& curBoard,
                   const std::vector<Board>& newBoards);

// Sum up the new prestige points (gem discount) gained by each move
Scores countPrestige(const Moves& moves, const player_id_t pid, const Board& curBoard,
                     const std::vector<Board>& newBoards);

// Award a point to a move if it leads to a winning board for this player
Scores winCondition(const Moves& moves, const player_id_t pid, const Board& curBoard,
                    const std::vector<Board>& newBoards);

// Sum up total no. of player gems:
Scores countGems(const Moves& moves, const player_id_t pid, const Board& curBoard,
                 const std::vector<Board>& newBoards);

// Award points based on how many moves we have (more moves->more 'mobility')
Scores countMoves(const Moves& moves, const player_id_t pid, const Board& curBoard,
                  const std::vector<Board>& newBoards);

// Reward BUY moves based on how rare the gem they give is
Scores monopolizeGems(const Moves& moves, const player_id_t pid, const Board& curBoard,
                      const std::vector<Board>& newBoards);

// Reward RESERVE moves that keep information hidden from other player, based on deck type
Scores preferWildcards(const Moves& moves, const player_id_t pid, const Board& curBoard,
                       const std::vector<Board>& newBoards);

// Penalize TAKE moves that return gems
Scores countReturns(const Moves& moves, const player_id_t pid, const Board& curBoard,
                    const std::vector<Board>& newBoards);

// Penalize move by round number
Scores preferShortGame(const Moves& moves, const player_id_t pid, const Board& curBoard,
                       const std::vector<Board>& newBoards);

}  // namespace