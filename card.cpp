//
// Created by eitan on 11/19/15.
//

#include <cassert>
#include <iostream>

#include "card.h"

namespace grandeur {

std::ostream&
operator<<(std::ostream& os, const Card& card)
{
    static constexpr const char* deckName[3] = { "L", "H", "M" };

    os << "card: " << deckName[card.id_.type_] << " " << card.id_.seq_ << "  ";
    os << "cost: " << card.cost_ << "  ";
    os << "provides: " << color2char[card.color_] << "  ";
    os << "points: " << card.points_;

    return os;
}

} // namespace