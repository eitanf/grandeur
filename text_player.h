// An implementation of a simple text UI that lets a player pick a valid move from a list
// Uses standard output for output and standard input for input.
//
// Created by eitan on 12/4/15.
//

#pragma once

#include "player.h"

namespace grandeur {

class TextPlayer final : public Player {
  public:
    TextPlayer(player_id_t pid);
    virtual GameMove getMove(const Board& board, const Cards& hidden, const Moves& legal) const;

    void moveUpdater(const Board& board, player_id_t pid, const GameMove& mv);

    bool firstTime_;
};

}  // namespace
