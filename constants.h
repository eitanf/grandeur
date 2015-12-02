//
// This header defines various game constants that affect the rules.
// Created by eitan on 11/19/15.
//

#pragma once

#include <cstdint>

namespace grandeur {

// Maximum no. of players that can participate in a game:
static constexpr auto MAX_NPLAYER = 4;

// How many gems a player can own at once (or fewer)
static constexpr auto MAX_PLAYER_GEMS = 10;

// How many cards may be reserved by a single player, in total
static constexpr auto MAX_PLAYER_RESERVES = 3;

// How many gems can a player take if they're all of different colors
static constexpr auto DIFFERENT_COLOR_GEMS = 3;

// How many gems can one take of a single color
static constexpr auto SAME_COLOR_GEMS = 2;

// How many table gems of a single color must we have to take SAME_COLOR_GEMS:
static constexpr auto MIN_SAME_COLOR_TABLE_GEMS = SAME_COLOR_GEMS + 2;

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

// No. of cards from each deck type in the initial board
static constexpr unsigned INITIAL_DECK_NCARD = 4;


}  // namespace