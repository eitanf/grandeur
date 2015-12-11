// An implementation of a simple "AI" player that picks the "best" move given a static
// scoring function for moves.
//
// Created by eitan on 12/6/15.
//

#pragma once

#include "player.h"
#include "eval.h"

namespace grandeur {

class GreedyPlayer final : public Player {
  public:
    GreedyPlayer(const evaluator_t& eval, player_id_t pid)
            : Player(pid), evaluator_(eval)
    {}

    virtual GameMove getMove(const Board& board, const Cards& hidden, const Moves& legal) const;

  private:
    evaluator_t evaluator_;
};

}  // namespace
