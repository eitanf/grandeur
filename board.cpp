//
// Created by eitan on 11/19/15.
//

#include "board.h"
#include "move_notifier.h"

#include <cassert>
#include <iostream>

using namespace std;

namespace grandeur {

//////////////////////////////////////////////////////////////////////////////////////
Board::Board(unsigned nplayer, const Cards& initialCards, const Nobles& initialNobles)
  : nplayer_(nplayer), cards_(initialCards), nobles_(initialNobles), purchased_(),
    tableGems_(g_gem_allocation[nplayer]), playerGems_(nplayer),
    playerPrestige_(nplayer), playerReserves_(nplayer), playerPoints_(nplayer),
    remainingCards_(), round_(1)
{
    remainingCards_[LOW] = deckCount(LOW, g_deck) - deckCount(LOW, cards_);
    remainingCards_[MEDIUM] = deckCount(MEDIUM, g_deck) - deckCount(MEDIUM, cards_);
    remainingCards_[HIGH] = deckCount(HIGH, g_deck) - deckCount(HIGH, cards_);
}


//////////////////////////////////////////////////////////////////////////////////////
MoveStatus
Board::takeGems(player_id_t pid, const Gems& gems)
{
    //// First, check for bugs or bad inputs:
    assert(pid < nplayer_);
    assert(totalGameGems() == g_gem_allocation[nplayer_]);
    assert(playerGems_[pid].totalGems() <= MAX_PLAYER_GEMS);

    // Can't take yellows:
    if (gems.getCount(YELLOW) > 0) {
        return TAKING_YELLOW;
    }

    // Can't take more than the table has:
    if ((tableGems_ - gems).hasNegatives()) {
        return INSUFFICIENT_TABLE_GEMS;
    }

    const auto maxcl = gems.maxColor();
    const auto maxct = gems.getCount(maxcl);
    if (maxct == SAME_COLOR_GEMS
     && tableGems_.getCount(maxcl) < MIN_SAME_COLOR_TABLE_GEMS) {
        return INSUFFICIENT_TABLE_GEMS;
    }

    // Can't return more than player has:
    if ((gems + playerGems_[pid]).hasNegatives()) {
        assert(!playerGems_[pid].hasNegatives());
        return INSUFFICIENT_GEMS_TO_RETURN;
    }

    // Can't take more than the max amount of gems allowed to own:
    const auto postCount = (gems + playerGems_[pid]).totalGems();
    if (postCount > MAX_PLAYER_GEMS) {
        return TOO_MANY_GEMS;
    }

    //// OK, now that we're taking the overall right amount of gems, ensure that the
    // gem combination is legal. Usually that means two gems of one color or one each
    // of three colors. But if we hit MAX_PLAYER_GEMS, we can return some to stay at limit

    if (postCount < MAX_PLAYER_GEMS) {
        if (gems.hasNegatives()
         || (gems.positiveColors() == 1 && maxct != SAME_COLOR_GEMS)
         || (gems.positiveColors() != 1 &&
             (gems.positiveColors() != DIFFERENT_COLOR_GEMS
                     || gems.totalGems() != DIFFERENT_COLOR_GEMS))) {
            return WRONG_NUMBER_OF_GEMS;
        }
    }
    else {   // We're at MAX_PLAYER_GEMS, can be more flexible on how many colors are active:
        if (maxct > SAME_COLOR_GEMS
         || (maxct == SAME_COLOR_GEMS && gems.positiveColors() > 1)) {
            return WRONG_NUMBER_OF_GEMS;
        }
    }

    // Phew, everything seems in order. Let's take those gems!
    playerGems_[pid] += gems;
    tableGems_ -= gems;

    return LEGAL_MOVE;
}

//////////////////////////////////////////////////////////////////////////////////////
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
    assert(replacement.isNull() || remainingCards_[replacement.id_.type_] > 0);

    // Ensure that replacement is only needed for table cards:
    assert(replacement.isNull() || cardIn(cid, cards_));

    // Ensure that buying a card from reserves means no replacement card:
    assert((!cardIn(cid, hidden) && !cardIn(cid, playerReserves_[pid])) ||
           (replacement.isNull()));

    // Ensure replacement is legitimate (not previously seen):
    assert(!replacement.isWild());
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


//////////////////////////////////////////////////////////////////////////////////////
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
    assert(!card.isNull());
    assert(!card.isWild());

    // Ensure that any replacement card comes from non-empty deck
    assert(replacement.isNull() || remainingCards_[replacement.id_.type_] > 0);

    // Ensure replacment is legitimate (not previously seen):
    assert(!replacement.isWild());
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
    if (!(replacement.isNull() || cardIn(card.id_, cards_))) {
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


//////////////////////////////////////////////////////////////////////////////////////
// A game is over when a player reached MIN_WIN_POINTS or when all table cards
// have been exhuasted
bool
Board::gameOver() const
{
    return (*max_element(begin(playerPoints_), end(playerPoints_)) >= MIN_WIN_POINTS
         || cards_.empty());
}


//////////////////////////////////////////////////////////////////////////////////////
// Which player has the most prestige points?
player_id_t
Board::leadingPlayer() const
{
    player_id_t which = 0;
    gem_count_t maxs = playerPoints_[which];
    for (auto i = which + 1; i < nplayer_; ++i) {
        if (playerPoints_[i] > maxs) {
            which = i;
            maxs = playerPoints_[i];
        } else if (playerPoints_[i] == maxs) {
            return nplayer_ + 1;
        }
    }
    return which;
}


//////////////////////////////////////////////////////////////////////////////////////
const Cards&
Board::tableCards() const
{
    assert(cards_.size() <= INITIAL_DECK_NCARD * NDECKS);
    return cards_;
}


//////////////////////////////////////////////////////////////////////////////////////
const Cards&
Board::playerReserves(player_id_t pid) const
{
    assert(playerReserves_.at(pid).size() <= MAX_PLAYER_RESERVES);
    return playerReserves_.at(pid);
}


//////////////////////////////////////////////////////////////////////////////////////
const Gems&
Board::tableGems() const
{
    assert(!tableGems_.hasNegatives()); // Not too little gems
    assert(!((g_gem_allocation[nplayer_] - tableGems_).hasNegatives())); // And not too many
    return tableGems_;
}


//////////////////////////////////////////////////////////////////////////////////////
const Gems&
Board::playerGems(player_id_t pid) const
{
    assert(!playerGems_[pid].hasNegatives()); // Not too little gems
    assert(!((g_gem_allocation[nplayer_] - playerGems_.at(pid)).hasNegatives())); // And not too many
    return playerGems_.at(pid);
}


//////////////////////////////////////////////////////////////////////////////////////
unsigned
Board::remainingCards(unsigned deck) const
{
    assert(deck < NDECKS);
    return remainingCards_[deck];
}


//////////////////////////////////////////////////////////////////////////////////////
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
    auto balance = playerGems_[pid].actualCost(where->cost_ - playerPrestige_[pid]);
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
    checkNobles(pid);

    return LEGAL_MOVE;
}

//////////////////////////////////////////////////////////////////////////////////////
// Update pile and remainingCards_ after a table card has been purchased or reserved
void
Board::removeCard(Cards& pile, typename Cards::iterator& where, const Card& replacement)
{
    if (replacement.isNull()) {
        pile.erase(where);
    } else {
        *where = replacement;
        assert(remainingCards_[replacement.id_.type_] > 0);
        --remainingCards_[replacement.id_.type_];
    }
}


//////////////////////////////////////////////////////////////////////////////////////
void
Board::checkNobles(player_id_t pid)
{
    for (auto iter = nobles_.begin(); iter != nobles_.end(); ) {
        if (!(playerPrestige_[pid] - iter->cost_).hasNegatives()) {
            playerPoints_[pid] += iter->points_;
            MoveNotifier::instance().notifyObservers(MoveEvent::NOBLE_WON, *this, pid, *iter);
            iter = nobles_.erase(iter);
        } else {
            ++iter;
        }

    }

}


//////////////////////////////////////////////////////////////////////////////////////
// Sum up all the gems of all players + table. Should be constant.
const Gems
Board::totalGameGems() const
{
    Gems ret = tableGems();
    for (unsigned i = 0; i < nplayer_; ++i) {
        ret += playerGems(i);
    }
    return ret;
}


//////////////////////////////////////////////////////////////////////////////////////
std::ostream&
operator<<(std::ostream& os, const Board& board)
{
    os << "Available gems: " << board.tableGems() << "\n";

    os << "Available cards:\n";
    for (auto& c : board.tableCards()) {
        os << c << "\n";
    }

    os << "Available nobles:\n";
    for (auto& n : board.tableNobles()) {
        os << n << "\n";
    }

    os << "Remaining cards: ";
    os << "Low: " << board.remainingCards(LOW) << "   ";
    os << "Medium: " << board.remainingCards(MEDIUM) << "   ";
    os << "High: " << board.remainingCards(HIGH) << "\n";

    os << "Player stats:\n";
    for (player_id_t p = 0; p < board.playersNum(); ++p) {
        os << "Player " << p << ": ";
        os << "points: " << board.playerPoints(p) << "   ";
        os << "gems: " << board.playerGems(p) << "   ";
        os << "prestige: " << board.playerPrestige(p) << "   ";
        os << "reserved (visible):";
        for (auto const& c : board.playerReserves(p)) {
            os << "  " << c.id_;
        }
        os << "\n";
    }

    return os;
}

} // namespace