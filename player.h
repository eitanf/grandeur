// Abstract interface for game player (AI or human)
//
// Created by eitan on 12/2/15.
//

#pragma once

#include "move.h"

#include <functional>
#include <memory>
#include <string>
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

///////////////////////////////////////////////////////////
// Abstract player factory to generate Player* from name.
// This class is a singleton.

class PlayerFactory {
  public:
    using creator_t = std::function<const Player* (player_id_t)>;
    using name_t = const std::string;

    // We register Player classes with a unioque name, and a callback that
    // creates (and allocates) a new concrete Player instance
    void registerPlayer(name_t name, creator_t creator);

    // Create a new Player with a given player ID.
    // Returns nullptr if the name can't be found in the registry.
    const Player* create(name_t, player_id_t);

    static PlayerFactory& instance()
    {
        static PlayerFactory singleton;
        return singleton;
    }

    // Use this struct with a static dummy variable to automatically register your Player
    struct Registrator {
        Registrator(name_t name, creator_t creator) { instance().registerPlayer(name, creator); }
    };

  private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;

    PlayerFactory();
    ~PlayerFactory();
    PlayerFactory(const PlayerFactory&) = delete;
    PlayerFactory& operator=(const PlayerFactory&) = delete;
};


} // namespace
