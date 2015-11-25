//
// move.h/.cpp defines a game move and the routines to make it.
// Created by eitan on 11/18/15.
//

#pragma once

#include "constants.h"

namespace grandeur {

enum MoveType { TAKE_GEMS, BUY_CARD, RESERVE_CARD };


enum MoveStatus {
    LEGAL_MOVE = 0,
    TAKING_YELLOW,   // Can't ask to take a yellow token from table
    INSUFFICIENT_TABLE_GEMS, // Trying to take unavailable gems
    TOO_MANY_GEMS,  // Can't collect more than MAX_PLAYER_GEMS
    WRONG_NUMBER_OF_GEMS, // Can only take three different ones or two of the same
    INSUFFICIENT_GEMS_TO_RETURN, // Trying to returns gems you don't have
    BUY_WILDCARD,   // Trying to buy a non-specific card
    INSUFFICIENT_GEMS,  // Trying to buy a card without enough gems
    UNAVAILABLE_CARD,   // Attempt to buy a card not available to this player
    TOO_MANY_RESERVES  // Attempt to reserve a card beyond the 3 allowed.
};

// A Game move consists of one of three move types, with an associated payload for each.
// If the payload is gems, it can only be a TAKE_GEMS move type. If it's a card ID,
// It can be either a BUY_CARD or RESERVE_CARD.
struct GameMove {
    GameMove(const Gems& gems) : type_(MoveType::TAKE_GEMS), payload_(gems) {}
    GameMove(const Card& card, MoveType type) : type_(type), payload_(card) {}

    MoveType type_;
    union payload {
        const Gems gems_;   // In case of TAKE_GEMS
        const Card card_;  // Otherwise
        payload(const Gems& gems) : gems_(gems) {};
        payload(const Card& card) : card_(card) {};
    } payload_;
};

using player_id_t = unsigned;

class Board;

// Update the board status in response to game move (one of three types).
// Returns an error code if the move could not be carried out, LEGAL_MOVE otherwise.
// Receives my (move maker) player id, the actual move, my hidden reserves, and
// possibly a card to replace any other card purchased or reserved from the table ones.
MoveStatus makeMove(Board& board, player_id_t pid, GameMove mymove,
                    Cards& myhidden, Card replacement = NULL_CARD);


// Attempt to perform move on a copy of board, and check its legality.
MoveStatus isLegalMove(Board board, player_id_t pid, const GameMove& mymove, Cards myhidden);

/*


// legalMoves enumerates (almost) all possible legal moves for a given player.
// The function all of the board elements this player can see.
// The only legal move that this function will not identify is one where
// a player takes an excess over MAX_GEMS and then returns some. This move is
// however identified as legal by isLegalMove().
std::vector<GameMove>
legalMoves(const VisibleBoard& board, int player);

*/

// mainGaimLoop is the run an entire new game.
void mainGameLoop();

}