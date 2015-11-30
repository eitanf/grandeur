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
#include <iterator>
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

    // Copy a range of gems (for iterator type only, not for a pair of args as above):
    template <typename Iter>
    Gems(Iter begin, Iter end, typename std::iterator_traits<Iter>::iterator_category* = nullptr);

    // Increment/decrement gem count for one color:
    int inc(gem_color_t color) { return ++gems_[color]; }
    int dec(gem_color_t color) { return --gems_[color]; }

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

    // Returns the amount of gems from each color that need to be substracted from this
    // to pay for target. Automatically matches yellow gems in this with insufficient
    // gems in target as necessary. But the actual cost may still exceed this if there
    // are insufficient yellows.
    Gems actualCost(const Gems& target) const;

    friend std::ostream& operator<<(std::ostream&, const Gems&);


    Gems& operator+=(const Gems& rhs);
    Gems& operator-=(const Gems& rhs);

    friend Gems operator+(Gems lhs, const Gems& rhs) { lhs += rhs; return lhs; }
    friend Gems operator-(Gems lhs, const Gems& rhs) { lhs -= rhs; return lhs; }

    bool operator==(const Gems& rhs) const { return gems_ == rhs.gems_; }
    bool operator!=(const Gems& rhs) const { return !(*this == rhs); }

    static constexpr unsigned NCOLOR = 6;
  private:
    std::array<gem_count_t, NCOLOR> gems_;
};

template <typename Iter>
Gems::Gems(Iter begin, Iter end, typename std::iterator_traits<Iter>::iterator_category*)
  : gems_({ 0, 0, 0, 0, 0, 0 })
{
    size_t i = 0;
    for (auto iter = begin; iter != end; ++iter) {
        gems_.at(i++) = *iter;
    }
}


// A mapping from the no. of players (2--4) to the initial table gem allocation:
static constexpr const Gems g_gem_allocation[] = {
        Gems(),
        Gems(),
        { INITIAL_GEMS[2], INITIAL_GEMS[2], INITIAL_GEMS[2], INITIAL_GEMS[2], INITIAL_GEMS[2], YELLOW_COUNT },
        { INITIAL_GEMS[3], INITIAL_GEMS[3], INITIAL_GEMS[3], INITIAL_GEMS[3], INITIAL_GEMS[3], YELLOW_COUNT },
        { INITIAL_GEMS[4], INITIAL_GEMS[4], INITIAL_GEMS[4], INITIAL_GEMS[4], INITIAL_GEMS[4], YELLOW_COUNT },
};

} // namepscae