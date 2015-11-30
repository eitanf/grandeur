//
// Created by eitan on 11/18/15.
//

#include <cassert>
#include <iostream>

#include "move.h"
#include "board.h"

using namespace std;

namespace grandeur {

// Enumerate all legal moves that take gems of a single color:
static void
addSameColorGemMoves(vector<GameMove>& moves, player_id_t pid, const Board& board)
{
    if (board.playerGems(pid).totalGems() > MAX_PLAYER_GEMS - SAME_COLOR_GEMS) {
        return;
    }

    static constexpr const Gems twoGemsOfColor[] = {
            { 2, 0, 0, 0, 0 },
            { 0, 2, 0, 0, 0 },
            { 0, 0, 2, 0, 0 },
            { 0, 0, 0, 2, 0 },
            { 0, 0, 0, 0, 2 }
    };

    for (unsigned i = 0; i < Gems::NCOLOR; ++i) {
        if (board.tableGems().getCount(gem_color_t(i)) >= MIN_SAME_COLOR_TABLE_GEMS) {
            moves.push_back(GameMove(twoGemsOfColor[i]));
        }
    }
}


// Enumerate all legal moves that take gems of different colors (no returns):
static void
addDifferentColorGemMoves(vector<GameMove>& moves, player_id_t pid, const Board& board)
{
    if (board.playerGems(pid).totalGems() > MAX_PLAYER_GEMS - DIFFERENT_COLOR_GEMS) {
        return;
    }

    vector<gem_count_t> different = { 0, 0, 1, 1, 1 };

    // Try all permutations of three different gems to see which is valid:
    do {
        const Gems trygems = Gems(different.cbegin(), different.cend());
        if (!(board.tableGems() - trygems).hasNegatives()) {
            moves.push_back(GameMove(trygems));
        }
    } while (next_permutation(different.begin(), different.end()));
}


// Enumerate all the cards (table/reserves) we can afford to buy:
static void
addBuyCardMoves(vector<GameMove>& moves, player_id_t pid, const Board& board, const Cards& myhidden)
{
    const auto& gems = board.playerGems(pid);
    const auto& prestige = board.playerPrestige(pid);

    auto allCards = board.tableCards();
    allCards.insert(allCards.end(), board.playerReserves(pid).cbegin(),
                    board.playerReserves(pid).cend());
    allCards.insert(allCards.end(), myhidden.cbegin(), myhidden.cend());

    for (const auto& card : allCards) {
        const auto balance = gems.actualCost(card.cost_ - prestige);
        if (!((gems - balance).hasNegatives())) {
            moves.push_back(GameMove(card, MoveType::BUY_CARD));
        }
    }
}


// Enumerate all the cards that can be reserved:
static void
addReserveCardMoves(vector<GameMove>& moves, player_id_t pid, const Board& board, const Cards& myhidden)
{
    if (board.playerReserves(pid).size() + myhidden.size() >= MAX_PLAYER_RESERVES
        || board.playerGems(pid).totalGems() >= MAX_PLAYER_GEMS
        || board.tableGems().getCount(YELLOW) <= 0) {
        return;
    }

    // Reserves from table cards:
    for (const auto& card : board.tableCards()) {
         moves.push_back(GameMove(card, MoveType::RESERVE_CARD));
    }

    // Reserves from undealt cards:
    if (board.remainingCards(LOW) > 0) {
        moves.push_back(GameMove(LOW_CARD, MoveType::RESERVE_CARD));
    }
    if (board.remainingCards(MEDIUM) > 0) {
        moves.push_back(GameMove(LOW_CARD, MoveType::RESERVE_CARD));
    }
    if (board.remainingCards(HIGH) > 0) {
        moves.push_back(GameMove(LOW_CARD, MoveType::RESERVE_CARD));
    }
}


// Accumulate legal moves of all four types:
vector<GameMove>
legalMoves(const Board& board, player_id_t pid, const Cards& myHidden)
{
    vector<GameMove> ret;

    addSameColorGemMoves(ret, pid, board);
    addDifferentColorGemMoves(ret, pid, board);
    addBuyCardMoves(ret, pid, board, myHidden);
    addReserveCardMoves(ret, pid, board, myHidden);

    assert(!ret.empty());
    return ret;
}


// Evaluate a move on a throwaway board to check its legality:
MoveStatus
isLegalMove(Board board, player_id_t pid, const GameMove& move, Cards hidden)
{
    return makeMove(board, pid, move, hidden);
}


MoveStatus
makeMove(Board& board, player_id_t pid, const GameMove& mymove,
         Cards& myhidden, const Card& replacement)
{
    MoveStatus status;

    switch (mymove.type_) {
    case MoveType::TAKE_GEMS:
        status = board.takeGems(pid, mymove.payload_.gems_);
        break;

    case MoveType::BUY_CARD:
        status = board.buyCard(pid, mymove.payload_.card_.id_, myhidden, replacement);
        break;

    case MoveType::RESERVE_CARD:
        status = board.reserveCard(pid, mymove.payload_.card_, myhidden, replacement);
        break;
    }

    return status;
}




void mainGameLoop(/* Board, Deck, reserves[], Player[] */)
{
// Copy and shuffle deck.
    // initialize board with the counts of each type.
    // Pull four cards of each type and add to board. Perhaps do all this before loop.

// At the end of a turn, if anyone reached MIN_WIN_POINTS, find the maximal player and declare them winner
}

ostream& operator<<(ostream& os, const GameMove& mv)
{
    switch (mv.type_) {
    case TAKE_GEMS:
        os << "Take" << (mv.payload_.gems_.totalColors() == 1? "2" : "3") << ": ";
        os << mv.payload_.gems_;
        break;
    case BUY_CARD:
        os << "Buy: " << mv.payload_.card_.id_;
        break;
    case RESERVE_CARD:
        os << "Reserve: " << mv.payload_.card_.id_;
        break;
    }

    return os;
}


} // namespace
