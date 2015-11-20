//
// This header defines various game constants that affect the rules.
// Created by eitan on 11/19/15.
//

#pragma once

#include <cstdint>

namespace grandeur {

// How many gems a player can own at once (or fewer)
static constexpr auto MAX_PLAYER_GEMS = 10;

// How many cards may be reserved by a single player, in total
static constexpr auto MAX_PLAYER_RESERVES = 3;

// How many gems can a player take if they're all of different colors
static constexpr auto DIFFERENT_COLOR_GEMS = 3;

// How many gems can one take of a single color
static constexpr auto SAME_COLOR_GEMS = 2;

// We have three types of decks, based on their typical prestige point value
static constexpr unsigned NDECKS = 3;

// The prestige point value of a card or noble, which is accumulated toward winning.
using points_t = unsigned;

// The minimum amount of prestige points required to win
static constexpr points_t MIN_WIN_POINTS = 15;

using gem_count_t = int8_t;

// A mapping from no. of players (2--4) to no. of gems of each color in the board
static constexpr gem_count_t INITIAL_GEMS[] = { -1, -1, 4, 5, 7};

// Except yellow is always five:
static constexpr gem_count_t YELLOW_COUNT = 5;

// A mapping from no. of players (2--4) to total initial no. of nobles
static constexpr gem_count_t INITIAL_NOBLES[] = { -1, -1, 3, 4, 5};


// No. of cards from each deck type in the initial board
static constexpr unsigned INITIAL_DECK_CARDS_NUMBER = 4;


enum class MoveStatus {
    LEGAL = 0,
    TAKING_YELLOW,   // Can't ask to take a yellow token from table
    INSUFFICIENT_TABLE_GEMS, // Trying to take unavailable gems
    TOO_MANY_GEMS,  // Can't collect more than MAX_PLAYER_GEMS
    WRONG_NUMBER_OF_GEMS, // Can only take three different ones or two of the same
    INSUFFICIENT_GEMS_TO_RETURN, // Trying to returns gems you don't have
    BUY_WILDCARD,   // Trying to buy a non-specific card
    INSUFFICIENT_GEMS,  // Trying to buy a card without enough gems
    UNAVAILABLE_CARD,   // Attempt to buy a card not avalabile to this player
    TOO_MANY_RESERVES  // Attempt to reserve a card beyond the 3 allowed.
};


}  // namespace