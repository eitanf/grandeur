// An implementation of a Mini-Max algorithm player.
// The player is initialized with an evaluator function that scores each possible
// move for a given board state. At depth 1, this is identical to the greedy player--
// it just picks the highest-scoring move. At depth N, it picks the moves that produces
// the highest score sum for the next N moves, where the opponent's highest score in
// each subsequent move is deducted from the current move's score.
//
// Created by eitan on 12/8/15.
//

#pragma once

#include "player.h"
#include "eval.h"

namespace grandeur {

class MinimaxPlayer final : public Player {
  public:
    MinimaxPlayer(unsigned maxDepth, const evaluator_t& eval, player_id_t pid, score_t agingWeight = 1);

    virtual GameMove
    getMove(const Board& board, const Moves& legal) const;

  private:
    std::pair<unsigned, score_t>
    bestMoveN(player_id_t pid, unsigned depth, const Board& board, const Moves& legal) const;

    unsigned depth_;
    evaluator_t evaluator_;
    score_t agingWeight_;
};

}  // namespace
