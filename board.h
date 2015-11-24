// Board class holds all the visible elements of a game board: the cards that
// have been dealt, the resources and stats of each player, table resources,
// the nobles, and the reserved cards (that were visible when reserved).
//
// Created by eitan on 11/19/15.
//

#pragma once

#include "card.h"
#include "constants.h"
#include "gems.h"
#include "move.h"

#include <cassert>
#include <iosfwd>
#include <vector>

namespace grandeur {

class Board {
  public:
    // Construct Board with the available no. of cards from each deck.
    Board(unsigned nplayer, const Cards& initialCards/*, const Nobles& initialNobles*/);
    Board(const Board&) = default;
    Board(Board&&) = default;
    Board& operator=(const Board&) = default;
    Board& operator=(Board&&) = default;

    // Attempt to take gems from community (table) pile:
    MoveStatus takeGems(player_id_t pid, const Gems& gems);

    // Perform record keeping after purchasing a card, including the
    // adjustment of player statistics and remainingCards_.
    MoveStatus buyCard(player_id_t pid, CardID cid, Cards& hidden,
                       const Card& replacement = NULL_CARD);

    // Perform record keeping after reserving a card. If the CardID is not in
    // the table cards, it's assumed to be hidden.
    MoveStatus reserveCard(player_id_t pid, const Card& card, Cards& hidden,
                           const Card& replacement = NULL_CARD);

    //////////// Accessor functions:

    const Cards& tableCards() const;
    const Cards& playerReserves(player_id_t pid) const;

    const Gems& tableGems() const;
    const Gems& playerGems(player_id_t pid) const;

    const Gems& playerPrestige(player_id_t pid) const { return playerPrestige_.at(pid); }

    points_t playerPoints(player_id_t pid) const { return playerPoints_.at(pid); }

    unsigned remainingCards(unsigned deck) const;

    friend std::ostream& operator<<(const std::ostream&, const Board&);

  private:


    // Like buyCard, but for a card in a specific set of cards:
    MoveStatus buyCardFromPile(player_id_t pid, typename Cards::iterator where, Cards& pile,
                               const Card& replacement);

    // For debugging purposes:
    const Gems totalGameGems() const;

    int nplayer_;  // Total no. of players
    Cards cards_;  // Visible cards
    Cards purchased_; // A record of past purchased card for sanity checking
    Gems tableGems_;  // Community gems
    std::vector<Gems> playerGems_;   // The current resource count of each player
    std::vector<Gems> playerPrestige_;   // The permanent resource discount of each player
    std::vector<points_t> playerPoints_;   // How many points each player has.
    std::vector<Cards> playerReserves_;  // Which visible cards each players has reserved
    unsigned remainingCards_[NDECKS];  // How many cards remain of each deck type.
    void removeCard(Cards& pile, typename Cards::iterator& where, const Card& replacement);
};

std::ostream& operator<<(const std::ostream&, const Board&);

} // namespace