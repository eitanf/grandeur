//
// Created by eitan on 11/18/15.
//

#include <cassert>

#include "move.h"
#include "board.h"

namespace grandeur {



MoveStatus
isLegalMove(Board board, player_id_t pid, const GameMove& move, Cards hidden)
{
    return makeMove(board, pid, move, hidden);
}

/*

std::vector<GameMove>
legalMoves(const Cards& tableCards, const Cards& myCards,
           const Gems& tableGems, const Gems& myGems)

{
    // If totalGems() <= MAX-2 iterate over five colors to find take-2
    // If totalGems() <= MAX-3 iterate over three-different combos
    // Iterate over all table cards to identify buy opportunity
    // Iterate over all reserved cards to identify buy opportunity
    // If myReserves.length() < MAX_RESERVES and yellow > 0, iterate over table cards
    return nullptr;
}


 */

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

} // namespace