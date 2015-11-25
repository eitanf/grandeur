// noble.h: Definition of Noble data structure.
// A noble is nothing more than a prestige point boost with an associated cost.
//
// Created by eitan on 11/25/15.
//

#pragma once

#include "gems.h"

#include <iosfwd>

namespace grandeur {

struct Noble {
  public:
    constexpr Noble(const Gems& cost, points_t points)
      : cost_(cost), points_(points)
    {}

    Gems cost_;
    points_t points_;
};

std::ostream& operator<<(std::ostream&, const Noble&);

// Mapping from no. of players to no. of nobles initially allocated to board:
static constexpr const Gems g_noble_allocation[] = { 0, 0, 3, 4, 5 };

// Full set of nobles, from which some will be allocated to board randomly
static constexpr const Noble g_nobles[] = {
        { { 0, 0, 3, 3, 3 }, 3 },
        { { 3, 3, 3, 0, 0 }, 0 },
        { { 3, 3, 0, 0, 3 }, 3 },
        { { 0, 3, 3, 3, 0 }, 3 },
        { { 3, 0, 0, 3, 3 }, 3 },
        { { 4, 4, 0, 0, 0 }, 3 },
        { { 0, 4, 4, 0, 0 }, 3 },
        { { 0, 0, 4, 4, 0 }, 3 },
        { { 4, 0, 0, 0, 4 }, 3 }
};


} // namespace