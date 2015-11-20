//
// Created by eitan on 11/18/15.
//

#include <algorithm>
#include <iostream>

#include "gems.h"

namespace grandeur {

int Gems::totalGems() const
{
    return std::accumulate(gems_.cbegin(), gems_.cend(), 0);
}

long Gems::totalColors() const
{
    return std::count_if(gems_.cbegin(), gems_.cend(),
                         [](auto v){ return (v != 0);});
}

bool Gems::hasNegatives() const
{
    return std::any_of(gems_.cbegin(), gems_.cend(),
                       [](auto v){ return (v < 0);});
}

gem_count_t Gems::maxQuantity() const
{
    return *std::max_element(gems_.cbegin(), gems_.cend());
}

Gems Gems::operator-(const Gems& rhs) const
{
    Gems ret(*this);
    for (size_t i = 0; i < NCOLOR; ++i) {
        ret.gems_[i] -= rhs.gems_[i];
    }
    return ret;
}

Gems Gems::operator+(const Gems& rhs) const
{
    Gems ret(*this);
    for (size_t i = 0; i < NCOLOR; ++i) {
        ret.gems_[i] += rhs.gems_[i];
    }
    return ret;
}

std::ostream& operator<<(std::ostream& os, const Gems& gems)
{
    os << "W: " << int(gems.gems_[0]);
    os << " T: " << int(gems.gems_[1]);
    os << " G: " << int(gems.gems_[2]);
    os << " R: " << int(gems.gems_[3]);
    os << " B: " << int(gems.gems_[4]);
    os << " Y: " << int(gems.gems_[5]);
    return os;
}

}