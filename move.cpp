//
// Created by eitan on 11/18/15.
//

#include <cassert>
#include <iostream>

#include "move.h"
#include "board.h"
#include "player.h"
#include "move_notifier.h"

using namespace std;

namespace grandeur {

bool GameMove::operator==(const GameMove& rhs) const
{
    if (type_ != rhs.type_) {
        return false;
    }
    if (type_ == MoveType::TAKE_GEMS) {
        return payload_.gems_ == rhs.payload_.gems_;
    } else {
        return payload_.card_ == rhs.payload_.card_;
    }
}


ostream& operator<<(ostream& os, const GameMove& mv)
{
    switch (mv.type_) {
    case TAKE_GEMS:
        os << "Take: " << mv.payload_.gems_;
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


// Test whether a combination of gems we're trying to take are too many gems for
// the given table gems
bool
isLegalTake(const Gems& takeGems, const Gems& tableGems, const Gems& myGems)
{
    // Are we taking more gems than the table has?
    if ((tableGems - takeGems).hasNegatives()) {
        return false;
    }
    // Are we returning more gems than we have?
    if ((myGems + takeGems).hasNegatives()) {
        return false;
    }
    // Are we taking too many gems of the same color?
    const auto maxcl = takeGems.maxColor();
    if (takeGems.getCount(maxcl) == SAME_COLOR_GEMS
            && tableGems.getCount(maxcl) < MIN_SAME_COLOR_TABLE_GEMS) {
        return false;
    }

    return true;
}


// For a given vector of gem-count combinations, search all permutation of this
// combination to find legal gem-taking permutations, and add them to moves.
// A gem-count combination is an ascending-sorted array of five counts, representing
// How many gems of each colors to take (where the actual colors vary by permutation).
static void
addTakeGemCombination(Moves& moves, player_id_t pid, const Board& board,
                      vector<gem_count_t> counts)
{
    do {
        const Gems trygems = Gems(begin(counts), end(counts));
        if (isLegalTake(trygems, board.tableGems(), board.playerGems(pid))) {
            moves.push_back(GameMove(trygems));
        }
    } while (next_permutation(begin(counts), end(counts)));

}

// Enumerate all possible combinations of taking gems that match the amount
// of gems player already has.
static void
addTakeGemMoves(Moves& moves, player_id_t pid, const Board& board)
{
    static const vector<gem_count_t> sameColor =   {  0,  0,  0,  0,  2 };
    static const vector<gem_count_t> diffColors =  {  0,  0,  1,  1,  1 };
    static const vector<gem_count_t> netAddZero1 = { -2,  0,  0,  0,  2 };
    static const vector<gem_count_t> netAddZero2 = { -1, -1,  0,  1,  1 };
    static const vector<gem_count_t> netAddOne1 =  { -1,  0,  0,  0,  2 };
    static const vector<gem_count_t> netAddOne2 =  { -1, -1,  1,  1,  1 };
    static const vector<gem_count_t> netAddTwo1 =  { -1,  0,  1,  1,  1 };
    static const vector<gem_count_t> netAddTwo2 =  {  0,  0,  0,  1,  1 };

    const auto ngems = board.playerGems(pid).totalGems();

    // Enumerate all legal moves that take gems of a single color:
    if (ngems <= MAX_PLAYER_GEMS - SAME_COLOR_GEMS) {
        addTakeGemCombination(moves, pid, board, sameColor);
    }

    // Enumerate all legal moves that take gems of different colors (no returns):
    if (ngems <= MAX_PLAYER_GEMS - DIFFERENT_COLOR_GEMS) {
        addTakeGemCombination(moves, pid, board, diffColors);
    }

    // Enumerate all legal moves that take and return gems (net zero change):
    if (ngems == MAX_PLAYER_GEMS) {
        addTakeGemCombination(moves, pid, board, netAddZero1);
        addTakeGemCombination(moves, pid, board, netAddZero2);
    }

    // Enumerate all legal moves that take and return gems (net one gem taken):
    if (ngems == MAX_PLAYER_GEMS - 1) {
        addTakeGemCombination(moves, pid, board, netAddOne1);
        addTakeGemCombination(moves, pid, board, netAddOne2);
    }

    // Enumerate all legal moves that take and return gems (net two gems taken):
    if (ngems == MAX_PLAYER_GEMS - 2) {
        addTakeGemCombination(moves, pid, board, netAddTwo1);
        addTakeGemCombination(moves, pid, board, netAddTwo2);
    }
}


// Enumerate all the cards (table/reserves) we can afford to buy:
static void
addBuyCardMoves(Moves& moves, player_id_t pid, const Board& board, const Cards& myhidden)
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
            assert(!card.isWild());
            assert(!card.isNull());
            moves.push_back(GameMove(card, MoveType::BUY_CARD));
        }
    }
}


// Enumerate all the cards that can be reserved:
static void
addReserveCardMoves(Moves& moves, player_id_t pid, const Board& board, const Cards& myhidden)
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
        moves.push_back(GameMove(MEDIUM_CARD, MoveType::RESERVE_CARD));
    }
    if (board.remainingCards(HIGH) > 0) {
        moves.push_back(GameMove(HIGH_CARD, MoveType::RESERVE_CARD));
    }
}


// Accumulate legal moves of all four types:
Moves
legalMoves(const Board& board, player_id_t pid, const Cards& myHidden)
{
    Moves ret;

    addTakeGemMoves(ret, pid, board);
    addBuyCardMoves(ret, pid, board, myHidden);
    addReserveCardMoves(ret, pid, board, myHidden);

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
        assert(!mymove.payload_.card_.isWild());
        assert(!mymove.payload_.card_.isNull());
        status = board.buyCard(pid, mymove.payload_.card_.id_, myhidden, replacement);
        break;

    case MoveType::RESERVE_CARD:
        assert(!mymove.payload_.card_.isNull());
        status = board.reserveCard(pid, mymove.payload_.card_, myhidden, replacement);
        break;
    }

    return status;
}


// TODO: Bypass mode that skips all the error checks in make move, if a move is forced to be legal.
player_id_t
mainGameLoop(Board& board, Cards& deck, Players& players)
{
    Cards hiddenReserves[MAX_NPLAYER];
    MoveStatus ok = LEGAL_MOVE;

    MoveNotifier::instance().notifyObservers(MoveEvent::GAME_BEGAN, board, 0);

    bool stalemate = false;
    int noBuyCount = 0;  // Have many turns have we gone without any BUY_CARD?
    static const auto MAX_NO_BUY_ROUNDS = 30;

    while (!board.gameOver() && !stalemate) {

        for (player_id_t pid = 0; pid < board.playersNum(); ++pid) {
            stalemate = true;
            ++noBuyCount;
            const auto legal = legalMoves(board, pid, hiddenReserves[pid]);
            if (legal.empty()) {
                continue;
            } else {
              if (noBuyCount < MAX_NO_BUY_ROUNDS) {
                  stalemate = false;
              }
            }

            auto pMove = players[pid]->getMove(board, hiddenReserves[pid], legal);
            if (pMove.type_ == MoveType::BUY_CARD) {
                noBuyCount = 0;
            }

            // Find replacement card if buying/reserving from table:
            Card replacement = NULL_CARD;
            if (pMove.type_ != MoveType::TAKE_GEMS
             && cardIn(pMove.payload_.card_.id_, board.tableCards())) {
                replacement = popFromDeck(pMove.payload_.card_.id_.type_, deck);
            }

            // find first available card if reserving from undealt cards:
            if (pMove.type_ == MoveType::RESERVE_CARD && pMove.payload_.card_.isWild()) {
                auto card = popFromDeck(pMove.payload_.card_.id_.type_, deck);
                assert(!card.isNull());
                auto newMove = GameMove(card, pMove.type_);
                ok = makeMove(board, pid, newMove, hiddenReserves[pid], replacement);
            } else {
                ok = makeMove(board, pid, pMove, hiddenReserves[pid], replacement);
            }

            assert(ok == LEGAL_MOVE);
            MoveNotifier::instance().notifyObservers(
                    MoveEvent::MOVE_TAKEN, board, pid, { pMove });
            if (pid == board.playersNum() - 1) {
                board.completedRound();
            }
        }
    }

    // End of game: find winner:
    const auto winner = board.leadingPlayer();
    if (winner < board.playersNum()) {
        MoveNotifier::instance().notifyObservers(MoveEvent::GAME_WON, board, winner);
    } else {
        MoveNotifier::instance().notifyObservers(MoveEvent::TIE, board, winner);
    }
    return winner;
}


} // namespace