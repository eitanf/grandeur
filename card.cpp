//
// Created by eitan on 11/19/15.
//

#include <iostream>

#include "card.h"

namespace grandeur {

std::ostream&
operator<<(std::ostream& os, const CardID& id)
{
    static constexpr const char* deckName[3] = {"L", "M", "H"};
    os << deckName[id.type_] << "," << id.seq_;
    return os;
}


std::ostream&
operator<<(std::ostream& os, const Card& card)
{
    os << "id: " << card.id_ << "   ";
    os << "cost: " << card.cost_ << "   ";
    os << "provides: " << color2char[card.color_] << "   ";
    os << "points: " << card.points_;

    return os;
}


} // namespace