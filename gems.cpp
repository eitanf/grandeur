//
// Created by eitan on 11/18/15.
//

#include <algorithm>
#include <cassert>
#include <iostream>
#include <iomanip>

#include "gems.h"

namespace grandeur {


int Gems::totalGems() const
{
    return std::accumulate(gems_.cbegin(), gems_.cend(), 0);
}

long Gems::positiveColors() const
{
    return std::count_if(gems_.cbegin(), gems_.cend(),
                         [](auto v){ return (v > 0);});
}

bool Gems::hasNegatives() const
{
    return std::any_of(gems_.cbegin(), gems_.cend(),
                       [](auto v){ return (v < 0);});
}

gem_color_t Gems::maxColor() const
{
    return gem_color_t(std::distance(gems_.cbegin(),
                                     std::max_element(gems_.cbegin(), gems_.cend())));
}


// Target may have negative-cost colors if we have "too much" discount.
Gems Gems::actualCost(const Gems& cost) const
{
    assert(!hasNegatives() && "Can't pay with negative gems");
    assert(cost.gems_[YELLOW] == 0 && "Can't require a target with yellow");
    Gems ret;

    for (int color = 0; color < NCOLOR - 1; ++color) {
        const auto positiveCost = std::max(cost.gems_[color], gem_count_t(0));
        ret.gems_[color] = std::min(gems_[color], positiveCost);
        ret.gems_[YELLOW] += std::max(0, positiveCost - gems_[color]);
    }
    return ret;
}


Gems& Gems::operator-=(const Gems& rhs)
{
    for (size_t i = 0; i < NCOLOR; ++i) {
        this->gems_[i] -= rhs.gems_[i];
    }
    return *this;
}

Gems& Gems::operator+=(const Gems& rhs)
{
    for (size_t i = 0; i < NCOLOR; ++i) {
        this->gems_[i] += rhs.gems_[i];
    }
    return *this;
}

std::ostream& operator<<(std::ostream& os, const Gems& gems)
{
    os << "W: " << int(gems.gems_[0]);
    os << "  T: " << int(gems.gems_[1]);
    os << "  G: " << int(gems.gems_[2]);
    os << "  R: " << int(gems.gems_[3]);
    os << "  B: " << int(gems.gems_[4]);
    os << "  Y: " << int(gems.gems_[5]);
    return os;
}

} // namespace