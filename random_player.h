// An implementation of a simple "AI" player that picks any legal move randomly.
// Note: you can set the see for the PRNG with the environment variable GRANDEUR_PLAYER_SEED
//
// Created by eitan on 12/2/15.
//

#pragma once

#include "player.h"

namespace grandeur {

class RandomPlayer final : public Player {
  public:
    RandomPlayer(player_id_t pid) : Player(pid) {}
    virtual GameMove getMove(const Board& board, const Cards& hidden, const Moves& legal) const;
};

}  // namespace
