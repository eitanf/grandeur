//
// Created by eitan on 11/19/15.
//

#include "board.h"

#include <cassert>
#include <iostream>

using namespace std;

namespace grandeur {

Board::Board(unsigned nplayer, const Cards& initialCards)
  : nplayer_(nplayer), cards_(initialCards), purchased_(),
    tableGems_(g_gem_allocation[nplayer]), playerGems_(nplayer),
    playerPrestige_(nplayer), playerReserves_(nplayer), playerPoints_(nplayer),
    remainingCards_()
{
    remainingCards_[LOW] = deckCount(LOW, g_deck) - deckCount(LOW, cards_);
    remainingCards_[MEDIUM] = deckCount(MEDIUM, g_deck) - deckCount(MEDIUM, cards_);
    remainingCards_[HIGH] = deckCount(HIGH, g_deck) - deckCount(HIGH, cards_);
}

MoveStatus
Board::takeGems(player_id_t pid, const Gems& gems)
{
    // First, check for bugs or bad inputs:
    assert(pid < nplayer_);
    assert(totalGameGems() == g_gem_allocation[nplayer_]);

    // Can't take yellows:
    if (gems.getCount(YELLOW) > 0) {
        return TAKING_YELLOW;
    }

    // Can't take more than the table has:
    if ((tableGems_ - gems).hasNegatives()) {
        return INSUFFICIENT_TABLE_GEMS;
    }

    // If taking single color, must ensure right amount and sufficient table gems:
    if (gems.totalColors() == 1) {
        for (size_t i = 0; i < Gems::NCOLOR; ++i) {
            const auto count = gems.getCount(gem_color_t(i));
            if (count != 0 && count != SAME_COLOR_GEMS) {
                return WRONG_NUMBER_OF_GEMS;
            }
            if (count != 0 && tableGems_.getCount(gem_color_t(i)) < MIN_SAME_COLOR_TABLE_GEMS) {
                return INSUFFICIENT_TABLE_GEMS;
            }
        }
    }
    else if (gems.totalColors() != DIFFERENT_COLOR_GEMS || gems.maxQuantity() != 1) {
        return WRONG_NUMBER_OF_GEMS;
    }

    if ((gems + playerGems_[pid]).hasNegatives()) {
        assert(!playerGems_[pid].hasNegatives());
        return INSUFFICIENT_GEMS_TO_RETURN;
    }

    // Player can't own too many gems:
    if ((gems + playerGems_[pid]).totalGems() > MAX_PLAYER_GEMS) {
        return TOO_MANY_GEMS;
    }

    // Phew, everything seems in order. Let's take those gems!
    playerGems_[pid] += gems;
    tableGems_ -= gems;

    return LEGAL_MOVE;
}


// Removes noble if won.
// Also removes card from visible cards or reserves, if found.
// If in deck and replacment isn't NULL_CARD, adds replacement to cards_.
MoveStatus
Board::buyCard(player_id_t pid, CardID cid, Cards& hidden, const Card& replacement)
{
    ///// First, check for bad programmatic inputs (bugs):
    assert(pid < nplayer_);
    assert(totalGameGems() == g_gem_allocation[nplayer_]);

    // Ensure that any replacement card comes from non-empty deck:
    assert(replacement == NULL_CARD || remainingCards_[replacement.id_.type_] > 0);

    // Ensure that replacement is only needed for table cards:
    assert(replacement == NULL_CARD || cardIn(cid, cards_));

    // Ensure that buying a card from reserves means no replacement card:
    assert((!cardIn(cid, hidden) && !cardIn(cid, playerReserves_[pid])) ||
           (replacement == NULL_CARD));

    // Ensure replacement is legitimate (not previously seen):
    assert(replacement.id_.seq_ != CardID::WILDCARD);
    assert(!cardIn(replacement.id_, cards_));
    assert(!cardIn(replacement.id_, purchased_));
    assert(!cardIn(replacement.id_, hidden));
    for (size_t i = 0; i < nplayer_; ++i) {
        assert(!cardIn(replacement.id_, playerReserves_[i]));
    }

    ///// Next, check for bad user inputs:
    if (cid.seq_ == CardID::WILDCARD) {
        return BUY_WILDCARD;
    }

    // Ensure card hasn't been purchased before:
    if(cardIn(cid, purchased_)) {
        return UNAVAILABLE_CARD;
    }

    //// Proceed to find the card to buy, check it's good, and if so, buy it:
    // Start with table cards:
    auto where = cardLocation(cid, cards_);
    if (where != end(cards_)) {
        return (buyCardFromPile(pid, where, cards_, replacement));
    }

    where = cardLocation(cid, playerReserves_[pid]);
    if (where != end(playerReserves_[pid])) {
        return (buyCardFromPile(pid, where, playerReserves_[pid], replacement));
    }

    where = cardLocation(cid, hidden);
    if (where != end(hidden)) {
        return (buyCardFromPile(pid, where, hidden, replacement));
    }

    // Couldn't find cid in any eligible pile, so it's a bad input.
    return UNAVAILABLE_CARD;
}


// Checks that the player hasn't exceed allowed reservations, and that
// enough yellow coins exist.
// If card not found board, add it to hidden.
MoveStatus
Board::reserveCard(player_id_t pid, const Card& card,
                   Cards& hidden, const Card& replacement)
{
    ///// First, check for bad programmatic inputs (bugs):
    assert(pid < nplayer_);
    assert(totalGameGems() == g_gem_allocation[nplayer_]);

    // Ensure that any replacement card comes from non-empty deck
    assert(replacement == NULL_CARD || remainingCards_[replacement.id_.type_] > 0);

    // Ensure replacment is legitimate (not previously seen):
    assert(replacement.id_.seq_ != CardID::WILDCARD);
    assert(!cardIn(replacement.id_, cards_));
    assert(!cardIn(replacement.id_, purchased_));
    assert(!cardIn(replacement.id_, hidden));
    for (size_t i = 0; i < nplayer_; ++i) {
        assert(!cardIn(replacement.id_, playerReserves_[i]));
    }

    ///// Next, check for bad user inputs:
    // Can't reserve too many cards:
    if (hidden.size() + playerReserves_[pid].size() >= MAX_PLAYER_RESERVES) {
        return TOO_MANY_RESERVES;
    }

    // Can't exceed player gem count:
    if (playerGems_[pid].totalGems() >= MAX_PLAYER_GEMS) {
        assert(playerGems_[pid].totalGems() == MAX_PLAYER_GEMS);
        return TOO_MANY_GEMS;
    }

    // Can't exceed table YELLOW count:
    if (tableGems_.getCount(YELLOW) <= 0) {
        assert(tableGems_.getCount(YELLOW) == 0);
        return INSUFFICIENT_TABLE_GEMS;
    }

    // Ensure card hasn't been purchased or reserved before:
    if (cardIn(card.id_, purchased_) || cardIn(card.id_, hidden)) {
        return UNAVAILABLE_CARD;
    }
    for (size_t i = 0; i < nplayer_; ++i) {
        if (cardIn(card.id_, playerReserves_[i])) {
            return UNAVAILABLE_CARD;
        }
    }

    // Ensure that replacement is only needed for table cards:
    if (!(replacement == NULL_CARD || cardIn(card.id_, cards_))) {
        return UNAVAILABLE_CARD;
    }


    //// Proceed to find the card in table cards or not (undealt deck):
    auto where = cardLocation(card.id_, cards_);

    if (where == end(cards_)) {  // An unseen card:
        hidden.push_back(card);
        assert(remainingCards_[card.id_.type_] > 0);
        --remainingCards_[card.id_.type_];
    }
    else {              // A table card:
        playerReserves_[pid].push_back(card);
        removeCard(cards_, where, replacement);
    }

    playerGems_[pid].inc(YELLOW);
    tableGems_.dec(YELLOW);
    return LEGAL_MOVE;
}


const Cards&
Board::tableCards() const
{
    assert(cards_.size() <= INITIAL_DECK_CARDS_NUMBER * NDECKS);
    return cards_;
}


const Cards&
Board::playerReserves(player_id_t pid) const
{
    assert(playerReserves_.at(pid).size() <= MAX_PLAYER_RESERVES);
    return playerReserves_.at(pid);
}


const Gems&
Board::tableGems() const
{
    assert(!tableGems_.hasNegatives()); // Not too little gems
    assert(!((g_gem_allocation[nplayer_] - tableGems_).hasNegatives())); // And not too many
    return tableGems_;
}


const Gems&
Board::playerGems(player_id_t pid) const
{
    assert(!playerGems_[pid].hasNegatives()); // Not too little gems
    assert(!((g_gem_allocation[nplayer_] - playerGems_.at(pid)).hasNegatives())); // And not too many
    return playerGems_.at(pid);
}


unsigned
Board::remainingCards(unsigned deck) const
{
    assert(deck < NDECKS);
    return remainingCards_[deck];
}


// buyCardsFromPile does the actual bookkeeping, once we've identified where we're buying
// the card from (table cards, reserves, etc.). We still have to check for adequate gems.
MoveStatus
Board::buyCardFromPile(player_id_t pid, typename Cards::iterator where,
                       Cards& pile, const Card& replacement)
{
    assert(where->cost_.getCount(YELLOW) == 0);
    assert(pid < nplayer_);

    // Compute how many gems we'll have left after the purchase, complement from yellows
    // as necessary.
    auto balance = playerGems_[pid].actualCost(where->cost_ - playerPrestige(pid));
    if ((playerGems_[pid] - balance).hasNegatives()) {
        return INSUFFICIENT_GEMS;
    }

    // OK, successful, update quantities:
    purchased_.push_back(*where);
    tableGems_ += balance;
    playerGems_[pid] -= balance;
    playerPrestige_[pid].inc(where->color_);
    playerPoints_[pid] += where->points_;
    removeCard(pile, where, replacement);

    return LEGAL_MOVE;
}

// Update pile and remainingCards_ after a table card has been purchased or reserved
void
Board::removeCard(Cards& pile, typename Cards::iterator& where, const Card& replacement)
{
    if (replacement == NULL_CARD) {
        pile.erase(where);
    } else {
        *where = replacement;
        assert(remainingCards_[replacement.id_.type_] > 0);
        --remainingCards_[replacement.id_.type_];
    }
}


const Gems
Board::totalGameGems() const
{
    Gems ret = tableGems();
    for (unsigned i = 0; i < nplayer_; ++i) {
        ret += playerGems(i);
    }
    return ret;
}


std::ostream&
operator<<(std::ostream& os, const Board& board)
{
    assert(false);
    return os;
}


} // namespace