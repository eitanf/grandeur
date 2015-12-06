//
// Created by eitan on 11/19/15.
//

#include <iostream>
#include <iomanip>

#include "card.h"

namespace grandeur {

Card popFromDeck(deck_t dt, Cards& cards) {
    const auto iter = std::find_if(cards.begin(), cards.end(),
                                   [dt](const Card& card){ return card.id_.type_ == dt; });
    if (cards.end() == iter) {
        return NULL_CARD;
    }
    else {
        auto ret = *iter;
        cards.erase(iter);
        return ret;
    }
}


std::ostream&
operator<<(std::ostream& os, const CardID& id)
{
    static constexpr const char* deckName[3] = {"L", "M", "H"};
    os << deckName[id.type_] << "," << std::setw(2) << id.seq_;
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