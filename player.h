// Abstract interface for game player (AI or human)
//
// Created by eitan on 12/2/15.
//

#pragma once

#include "move.h"

#include <vector>

namespace grandeur {

class Player {
  public:
    Player(player_id_t pid) : pid_(pid) {}
    virtual ~Player() = default;

    // Main interface Player must satisfy: pick a game move for a given board.
    virtual GameMove getMove(Board& board, Cards& hidden, const Moves& legal) = 0;

    player_id_t pid_;
};

using Players = std::vector<const Player*>;

} // namespace
