// Card: a class + utilities to represent a development card
//
// Created by eitan on 11/19/15.
//

#pragma once

#include "gems.h"
#include "constants.h"

#include <algorithm>
#include <iosfwd>
#include <utility>
#include <vector>

namespace grandeur {

enum deck_t { LOW = 0, MEDIUM = 1, HIGH = 2};

// A unique identifier for card consists of its deck type and a sequential number.
struct CardID {
  public:
    bool operator==(const CardID& rhs) const
    {
        return (type_ == rhs.type_ && seq_ == rhs.seq_);
    }

    using seq_t = int;

    deck_t type_;
    seq_t seq_;
};

std::ostream& operator<<(std::ostream&, const CardID&);


static constexpr CardID::seq_t WILDCARD = -1;  // Flag value for any card of deck.
static constexpr CardID::seq_t NULLCARD = -2;  // Flag value for unset card.

// Each card has a unique identifier, resource cost in gems, the permanent gem
// color it will discount, and a number of prestige points it awards (can be zero)
struct Card {
  public:
    constexpr Card(CardID id, const Gems& cost, gem_color_t color, points_t points)
      : id_(id), cost_(cost), color_(color), points_(points)
    {}
    Card(const Card&) = default;
    Card(Card&&) = default;
    Card& operator=(const Card&) = default;
    Card& operator=(Card&&) = default;

    bool operator==(const Card& rhs) const { return id_ == rhs.id_; }
    constexpr bool isNull() const { return id_.seq_ == NULLCARD; }
    constexpr bool isWild() const { return id_.seq_ == WILDCARD; }

    CardID id_;
    Gems cost_;
    gem_color_t color_;
    points_t points_;
};

std::ostream& operator<<(std::ostream&, const Card&);


// Define an "empty" card:
static constexpr Card NULL_CARD = { { LOW, NULLCARD }, { }, YELLOW, 0 };

// Define Wildcards, one per deck:
static constexpr Card LOW_CARD = { { LOW, WILDCARD }, { }, YELLOW, 0 };
static constexpr Card MEDIUM_CARD = { { MEDIUM, WILDCARD }, { }, YELLOW, 0 };
static constexpr Card HIGH_CARD = { { HIGH, WILDCARD }, { }, YELLOW, 0 };

//////////////////////////////////////////////////////////////////////////////
///// Operations on collections of cards (decks):

using Cards = std::vector<Card>;

// How many cards belong to a given deck type
template <class Container>
long deckCount(deck_t dt, const Container& cards)
{
    return std::count_if(std::begin(cards), std::end(cards), [dt](const Card& card) {
        return (card.id_.type_ == dt ? 1 : 0);
    });
}

// Return an iterator to a card collection for where a given card id is (or end() if not):
template <class Container>
auto cardLocation(CardID cid, Container&& cards)
{
    return (std::find_if(std::begin(std::forward<Container>(cards)),
                         std::end(std::forward<Container>(cards)),
                         [cid](const Card& card) {
        return cid == card.id_;
    }));
}

// Check if a given card is in a container of cards:
template <class Container>
bool cardIn(CardID cid, const Container& cards) {
    return (std::end(cards) != cardLocation(cid, cards));
}

// Find the first card of a given deck type in a deck, erase it and return it
// returns NULL_CARD if no more cards left of this deck type
Card popFromDeck(deck_t dt, Cards& cards);


// Global constant card list for the full set of game cards.
static constexpr const Card g_deck[] = {
        { { LOW, 0 }, { 1, 1, 1, 1, 0 }, BLACK, 0 },
        { { LOW, 1 }, { 1, 2, 1, 1, 0 }, BLACK, 0 },
        { { LOW, 2 }, { 2, 2, 0, 1, 0 }, BLACK, 0 },
        { { LOW, 3 }, { 0, 0, 1, 3, 1 }, BLACK, 0 },
        { { LOW, 4 }, { 0, 0, 2, 1, 0 }, BLACK, 0 },
        { { LOW, 5 }, { 2, 0, 2, 0, 0 }, BLACK, 0 },
        { { LOW, 6 }, { 0, 0, 3, 0, 0 }, BLACK, 0 },
        { { LOW, 7 }, { 0, 4, 0, 0, 0 }, BLACK, 1 },
        { { LOW, 8 }, { 1, 0, 1, 1, 1 }, TEAL, 0 },
        { { LOW, 9 }, { 1, 0, 1, 2, 1 }, TEAL, 0 },
        { { LOW, 10 }, { 1, 0, 2, 2, 0 }, TEAL, 0 },
        { { LOW, 11 }, { 0, 1, 3, 1, 0 }, TEAL, 0 },
        { { LOW, 12 }, { 1, 0, 0, 0, 2 }, TEAL, 0 },
        { { LOW, 13 }, { 0, 0, 2, 0, 2 }, TEAL, 0 },
        { { LOW, 14 }, { 0, 0, 0, 0, 3 }, TEAL, 0 },
        { { LOW, 15 }, { 0, 0, 0, 4, 0 }, TEAL, 1 },
        { { LOW, 16 }, { 0, 1, 1, 1, 1 }, WHITE, 0 },
        { { LOW, 17 }, { 0, 1, 2, 1, 1 }, WHITE, 0 },
        { { LOW, 18 }, { 0, 2, 2, 0, 1 }, WHITE, 0 },
        { { LOW, 19 }, { 3, 1, 0, 0, 1 }, WHITE, 0 },
        { { LOW, 20 }, { 0, 0, 0, 2, 1 }, WHITE, 0 },
        { { LOW, 21 }, { 0, 2, 0, 0, 2 }, WHITE, 0 },
        { { LOW, 22 }, { 0, 3, 0, 0, 0 }, WHITE, 0 },
        { { LOW, 23 }, { 0, 0, 4, 0, 0 }, WHITE, 1 },
        { { LOW, 24 }, { 1, 1, 0, 1, 1 }, GREEN, 0 },
        { { LOW, 25 }, { 1, 1, 0, 1, 2 }, GREEN, 0 },
        { { LOW, 26 }, { 0, 1, 0, 2, 2 }, GREEN, 0 },
        { { LOW, 27 }, { 1, 3, 1, 0, 0 }, GREEN, 0 },
        { { LOW, 28 }, { 2, 1, 0, 0, 0 }, GREEN, 0 },
        { { LOW, 29 }, { 0, 2, 0, 2, 0 }, GREEN, 0 },
        { { LOW, 30 }, { 0, 0, 0, 3, 0 }, GREEN, 0 },
        { { LOW, 31 }, { 0, 0, 0, 0, 4 }, GREEN, 1 },
        { { LOW, 32 }, { 1, 1, 1, 0, 1 }, RED, 0 },
        { { LOW, 33 }, { 2, 1, 1, 0, 1 }, RED, 0 },
        { { LOW, 34 }, { 2, 0, 1, 0, 2 }, RED, 0 },
        { { LOW, 35 }, { 1, 0, 0, 1, 3 }, RED, 0 },
        { { LOW, 36 }, { 0, 2, 1, 0, 0 }, RED, 0 },
        { { LOW, 37 }, { 2, 0, 0, 2, 0 }, RED, 0 },
        { { LOW, 38 }, { 3, 0, 0, 0, 0 }, RED, 0 },
        { { LOW, 39 }, { 4, 0, 0, 0, 0 }, RED, 1 },
        { { MEDIUM, 40 }, { 3, 2, 2, 0, 0 }, BLACK, 1 },
        { { MEDIUM, 41 }, { 3, 0, 3, 0, 2 }, BLACK, 1 },
        { { MEDIUM, 42 }, { 0, 1, 4, 2, 0 }, BLACK, 2 },
        { { MEDIUM, 43 }, { 0, 0, 5, 3, 0 }, BLACK, 2 },
        { { MEDIUM, 44 }, { 5, 0, 0, 0, 0 }, BLACK, 2 },
        { { MEDIUM, 45 }, { 0, 0, 0, 0, 6 }, BLACK, 3 },
        { { MEDIUM, 46 }, { 0, 2, 2, 3, 0 }, TEAL, 1 },
        { { MEDIUM, 47 }, { 0, 2, 3, 0, 3 }, TEAL, 1 },
        { { MEDIUM, 48 }, { 5, 3, 0, 0, 0 }, TEAL, 2 },
        { { MEDIUM, 49 }, { 2, 0, 0, 1, 4 }, TEAL, 2 },
        { { MEDIUM, 50 }, { 0, 5, 0, 0, 0 }, TEAL, 2 },
        { { MEDIUM, 51 }, { 0, 6, 0, 0, 0 }, TEAL, 3 },
        { { MEDIUM, 52 }, { 0, 0, 3, 2, 2 }, WHITE, 1 },
        { { MEDIUM, 53 }, { 2, 3, 0, 3, 0 }, WHITE, 1 },
        { { MEDIUM, 54 }, { 0, 0, 1, 4, 2 }, WHITE, 2 },
        { { MEDIUM, 55 }, { 0, 0, 0, 5, 3 }, WHITE, 2 },
        { { MEDIUM, 56 }, { 0, 0, 0, 5, 0 }, WHITE, 2 },
        { { MEDIUM, 57 }, { 6, 0, 0, 0, 0 }, WHITE, 3 },
        { { MEDIUM, 58 }, { 3, 0, 2, 3, 0 }, GREEN, 1 },
        { { MEDIUM, 59 }, { 2, 3, 0, 0, 2 }, GREEN, 1 },
        { { MEDIUM, 60 }, { 4, 2, 0, 0, 1 }, GREEN, 2 },
        { { MEDIUM, 61 }, { 0, 5, 3, 0, 0 }, GREEN, 2 },
        { { MEDIUM, 62 }, { 0, 0, 5, 0, 0 }, GREEN, 2 },
        { { MEDIUM, 63 }, { 0, 0, 6, 0, 0 }, GREEN, 3 },
        { { MEDIUM, 64 }, { 2, 0, 0, 2, 3 }, RED, 1 },
        { { MEDIUM, 65 }, { 0, 3, 0, 2, 3 }, RED, 1 },
        { { MEDIUM, 66 }, { 1, 4, 2, 0, 0 }, RED, 2 },
        { { MEDIUM, 67 }, { 3, 0, 0, 0, 5 }, RED, 2 },
        { { MEDIUM, 68 }, { 0, 0, 0, 0, 5 }, RED, 3 },
        { { MEDIUM, 69 }, { 0, 0, 0, 6, 0 }, RED, 3 },
        { { HIGH, 70 }, { 3, 3, 5, 3, 0 }, BLACK, 3 },
        { { HIGH, 71 }, { 0, 0, 0, 7, 0 }, BLACK, 4 },
        { { HIGH, 72 }, { 0, 0, 3, 6, 3 }, BLACK, 4 },
        { { HIGH, 73 }, { 0, 0, 0, 7, 3 }, BLACK, 5 },
        { { HIGH, 74 }, { 3, 0, 3, 3, 5 }, TEAL, 3 },
        { { HIGH, 75 }, { 7, 0, 0, 0, 0 }, TEAL, 4 },
        { { HIGH, 76 }, { 6, 3, 0, 0, 3 }, TEAL, 4 },
        { { HIGH, 77 }, { 7, 3, 0, 0, 0 }, TEAL, 5 },
        { { HIGH, 78 }, { 0, 3, 3, 5, 3 }, WHITE, 3 },
        { { HIGH, 79 }, { 0, 0, 0, 0, 7 }, WHITE, 4 },
        { { HIGH, 80 }, { 3, 0, 0, 3, 6 }, WHITE, 4 },
        { { HIGH, 81 }, { 3, 0, 0, 0, 7 }, WHITE, 5 },
        { { HIGH, 82 }, { 5, 3, 0, 3, 3 }, GREEN, 3 },
        { { HIGH, 83 }, { 0, 7, 0, 0, 0 }, GREEN, 4 },
        { { HIGH, 84 }, { 3, 6, 3, 0, 0 }, GREEN, 4 },
        { { HIGH, 85 }, { 0, 7, 3, 0, 0 }, GREEN, 5 },
        { { HIGH, 86 }, { 3, 5, 3, 0, 3 }, RED, 3 },
        { { HIGH, 87 }, { 0, 0, 7, 0, 0 }, RED, 4 },
        { { HIGH, 88 }, { 0, 3, 6, 3, 0 }, RED, 4 },
        { { HIGH, 89 }, { 0, 0, 7, 3, 0 }, RED, 5 }
};

} // namespace