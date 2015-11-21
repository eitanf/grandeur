//
// Created by eitan on 11/19/15.
//

#include "board.h"
#include <algorithm>

using namespace std;

namespace grandeur {

Board::Board(unsigned nplayer, const Cards& initialCards)
  : nplayer_(nplayer), cards_(initialCards), tableGems_(g_gem_allocation[nplayer]),
    playerGems_(nplayer), playerPrestige_(nplayer), playerReserves_(nplayer),
    playerPoints_(nplayer), remainingCards_()
{
    remainingCards_[LOW] = deckCount(g_deck, LOW) - deckCount(cards_, LOW);
    remainingCards_[MEDIUM] = deckCount(g_deck, MEDIUM) - deckCount(cards_, MEDIUM);
    remainingCards_[HIGH] = deckCount(g_deck, HIGH) - deckCount(cards_, HIGH);
}

MoveStatus Board::takeGems(Board::player_id_t pid, const Gems& gems)
{
    assert(pid < nplayer_);
}


// Removes noble if won.
// Also removes card from visible cards or reserves, if found.
// If in deck and replacment isn't NULL_CARD, adds replacement to cards_.
MoveStatus Board::buyCard(player_id_t pid, CardID cid, const Cards& hidden, Card replacement)
{
    // Ensure card is available for purchase. No replacement if from reserves
    assert(pid < nplayer_);
    assert(false && "replacement can happen from deck with no cards");
}


// Checks that the player hasn't exceed allowed reservations, and that
// enough yellow coins exist.
// If card not found board, add it to hidden.
MoveStatus Board::reserveCard(Board::player_id_t pid, CardID cid, Cards& hidden,
                              Card replacement)
{
    // Ensure card is available for reservation. Ensure undealt reserve means no replacement
    assert(pid < nplayer_);
    assert(false && "replacement can happen from deck with no cards");
}



const Cards& Board::tableCards() const
{
    assert(!cards_.empty());
    assert(cards_.size() <= INITIAL_DECK_CARDS_NUMBER * NDECKS);
    return cards_;
}

const Cards& Board::playerReserves(Board::player_id_t pid) const
{
    assert(playerReserves_.at(pid).size() <= MAX_PLAYER_RESERVES);
    return playerReserves_.at(pid);
}

const Gems& Board::tableGems() const
{
    assert(!tableGems_.hasNegatives()); // Not too little gems
    assert(!((g_gem_allocation[nplayer_] - tableGems_).hasNegatives())); // And not too many
    return tableGems_;
}


const Gems& Board::playerGems(Board::player_id_t pid) const
{
    assert(!playerGems_[pid].hasNegatives()); // Not too little gems
    assert(!((g_gem_allocation[nplayer_] - playerGems_.at(pid)).hasNegatives())); // And not too many
    return playerGems_.at(pid);
}

unsigned Board::remainingCards(unsigned deck) const
{
    assert(deck < NDECKS);
    return remainingCards_[deck];
}

std::ostream& operator<<(const std::ostream& ostream, const Board& board)
{
    assert(false);
}


} // namespace