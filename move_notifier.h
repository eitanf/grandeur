// MoveNotifier is a singleton that lets interested observers (e.g., players)
// register to receive notifications for any board changes. They register
// a callback function that gets the new state of the Board, and if a move was
// just made, the player who made it and the move.
// There are two special cases when NULL_MOVE is passed: At the beginning of the
// game, before any moves were made, and at the end of a game, with the winning
// player passed as the moving player's pid (or an index too large if stalemate)
//
// Created by eitan on 12/4/15.
//

#pragma once

#include "board.h"
#include "move.h"
#include "noble.h"

namespace grandeur {

enum class MoveEvent { GAME_BEGAN = 0,  // Start a game
                       MOVE_TAKEN,      // A game move was made
                       NOBLE_WON,       // A noble was won
                       GAME_WON,        // The game ended with a player winning
                       TIE              // The game ended with nobody winning
};


class MoveNotifier {
  public:
    // Extra information about the event that ocurred.
    union Payload {
        Payload(const GameMove& mv) : mv_(mv) {}
        Payload(const Noble& noble) : noble_(noble) {}
        Payload() : mv_(NULL_MOVE) {}

        GameMove mv_;
        Noble noble_;
    };

    using observer_t = std::function<
            void(MoveEvent, const Board&, player_id_t, const Payload&)>;

    static MoveNotifier& instance()
    {
        static MoveNotifier singleton;
        return singleton;
    }

    void registerObserver(observer_t observer)
    { observers_.push_back(observer); }

    void notifyObservers(MoveEvent event, const Board& board, player_id_t pid,
                         const Payload& payload = Payload())
    {
        for (auto obs : observers_) {
            obs(event, board, pid, payload);
        }
    }


  private:
    std::vector <observer_t> observers_;

    MoveNotifier() = default;

    MoveNotifier(const MoveNotifier&) = delete;

    MoveNotifier& operator=(const MoveNotifier&) = delete;
};


} // namespace