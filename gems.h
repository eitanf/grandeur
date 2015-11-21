//
// Declarations for a gem resource class. This class holds gems of different
// colors (with YELLOW being a special wildcard color). These collections can
// be added, substracted, and compared based on the game rules.
//
// Created by eitan on 11/18/15.
//

#pragma once

#include "constants.h"

#include <array>
#include <iosfwd>

namespace grandeur {

enum gem_color_t {
    WHITE = 0, TEAL = 1, GREEN = 2, RED = 3, BLACK = 4, YELLOW = 5
};

static constexpr char color2char[] = { 'W', 'T', 'G', 'R', 'B', 'Y' };


class Gems {
  public:
    // Catch-all constructor for gems
    template <typename... T>
    constexpr Gems(T... args) : gems_{static_cast<gem_count_t>(args)...} {}

    // How many gems we have in total (substracting negatives)
    int totalGems() const;

    // How many different gem colors have non-zero quantities?
    long totalColors() const;

    // Does any gem color appear in negative quantities?
    bool hasNegatives() const;

    // What is the maximum quantity of gems of any color?
    gem_count_t maxQuantity() const;

    gem_count_t getCount(gem_color_t color) const { return gems_.at(color); }

    bool empty() const { return (totalColors() == 0); }

    friend std::ostream& operator<<(std::ostream&, const Gems&);

    Gems operator-(const Gems& rhs) const;
    Gems operator+(const Gems& rhs) const;

    bool operator==(const Gems& rhs) const { return gems_ == rhs.gems_; }
    bool operator!=(const Gems& rhs) const { return !(*this == rhs); }

  private:
    static constexpr unsigned NCOLOR = 6;
    std::array<gem_count_t, NCOLOR> gems_;
};

// A mapping from the no. of players (2--4) to the initial table gem allocation:
static constexpr const Gems g_gem_allocation[] = {
        Gems(),
        Gems(),
        { INITIAL_GEMS[2], INITIAL_GEMS[2], INITIAL_GEMS[2], INITIAL_GEMS[2], INITIAL_GEMS[2], YELLOW_COUNT },
        { INITIAL_GEMS[3], INITIAL_GEMS[3], INITIAL_GEMS[3], INITIAL_GEMS[3], INITIAL_GEMS[3], YELLOW_COUNT },
        { INITIAL_GEMS[4], INITIAL_GEMS[4], INITIAL_GEMS[4], INITIAL_GEMS[4], INITIAL_GEMS[4], YELLOW_COUNT },
};

} // namepscae