//
// Created by eitan on 11/18/15.
//

#include <iostream>

#include "gems.h"

namespace grandeur {

int Gems::totalGems() const
{
    return 0;
}

int Gems::totalColors() const
{
    return 0;
}

bool Gems::hasNegatives() const
{
    return false;
}

bool Gems::maxQuantity() const
{
    return false;
}

std::ostream& operator<<(std::ostream& os, const Gems& gems)
{
    os << "W: " << int(gems.gems_[0]);
    os << "T: " << int(gems.gems_[1]);
    os << "G: " << int(gems.gems_[2]);
    os << "R: " << int(gems.gems_[3]);
    os << "B: " << int(gems.gems_[4]);
    os << "Y: " << int(gems.gems_[5]);
    return os;
}

}