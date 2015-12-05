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

///// Interface for an evaluation function.
// Receives the moves to be scored, the player id making the move, the
// board state before making the move, the board states after making each move,
// and the current stack of hidden reserve cards of the player.
using evaluator_t = std::function<Scores(const Moves& moves,
                                         const player_id_t pid,
                                         const Board& curBoard,
                                         std::vector<Board>& newBoards,
                                         const Cards& hidden)>;


// Combine multiple evaluators to one (additive)
// TODO: Change this into combineEvaluator function, with weights (default ones)
evaluator_t operator+(const evaluator_t& lhs, const evaluator_t& rhs);


// Compute scores for a given board state and evaluation function
// It'll automatically generate newBoards as necessary.
Scores computeScores(const evaluator_t eval,
                     const Moves& moves,
                     const player_id_t pid,
                     const Board& curBoard,
                     const Cards& hidden);


////////////////////////////////////////////////////////////////////
////////// Declarations for various evaluation functions:

// Sum up the points gained by each move:
Scores countPoints(const Moves& moves,
                   const player_id_t pid,
                   const Board& curBoard,
                   std::vector<Board>& newBoards,
                   const Cards& hidden);

}  // namespace