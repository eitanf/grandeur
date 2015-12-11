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

///////////////////////////////////////////////////////////////////
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


///////////////////////////////////////////////////////////////////
ostream& operator<<(ostream& os, const GameMove& mv)
{
    static constexpr const char* dname[] = { "LOW", "MEDIUM", "HIGH" };
    switch (mv.type_) {
    case TAKE_GEMS:
        os << "Take: " << mv.payload_.gems_;
        break;
    case BUY_CARD:
        os << "Buy: " << mv.payload_.card_.id_;
        break;
    case RESERVE_CARD:
        os << "Reserve: ";
        if (mv.payload_.card_.isWild()) {
            os << "Wildcard from deck " << dname[mv.payload_.card_.id_.type_];
        } else {
            os << mv.payload_.card_.id_;
        }
        break;
    }

    return os;
}

using Counts = array<gem_count_t, 5>;

///////////////////////////////////////////////////////////////////
// Test whether a combination of gems we're trying to take are too many gems for
// the given table gems
bool inline
isLegalTake(const Counts& take, const Counts& table, const Counts& mine)
{
    for (unsigned i = 0; i < Gems::NCOLOR - 1; ++i) {
        if ((take[i] > table[i])
        ||  (mine[i] + take[i] < 0)
        ||  (take[i] == SAME_COLOR_GEMS && table[i] < MIN_SAME_COLOR_TABLE_GEMS)) {
            return false;
        }
    }

    return true;
}


///////////////////////////////////////////////////////////////////
// For a given vector of gem-count combinations, search all permutation of this
// combination to find legal gem-taking permutations, and add them to moves.
// A gem-count combination is an ascending-sorted array of five counts, representing
// How many gems of each colors to take (where the actual colors vary by permutation).
static void
addTakeGemCombination(Moves& moves, player_id_t pid, const Board& board,
                      Counts counts, const Counts& table, const Counts& mine)
{
    do {
        if (isLegalTake(counts, table, mine)) {
            moves.push_back(GameMove(Gems(begin(counts), end(counts))));
        }
    } while (next_permutation(begin(counts), end(counts)));

}

///////////////////////////////////////////////////////////////////
// Enumerate all possible combinations of taking gems that match the amount
// of gems player already has.
static void
addTakeGemMoves(Moves& moves, player_id_t pid, const Board& board)
{
    static const Counts sameColor =   {  0,  0,  0,  0,  2 };
    static const Counts diffColors =  {  0,  0,  1,  1,  1 };
    static const Counts netAddZero1 = { -2,  0,  0,  0,  2 };
    static const Counts netAddZero2 = { -1, -1,  0,  1,  1 };
    static const Counts netAddOne1 =  { -1,  0,  0,  0,  2 };
    static const Counts netAddOne2 =  { -1, -1,  1,  1,  1 };
    static const Counts netAddTwo1 =  { -1,  0,  1,  1,  1 };
    static const Counts netAddTwo2 =  {  0,  0,  0,  1,  1 };

    const Counts table({ board.tableGems().getCount(gem_color_t(0)),
                         board.tableGems().getCount(gem_color_t(1)),
                         board.tableGems().getCount(gem_color_t(2)),
                         board.tableGems().getCount(gem_color_t(3)),
                         board.tableGems().getCount(gem_color_t(4))
                       });
    const Counts mine({ board.playerGems(pid).getCount(gem_color_t(0)),
                        board.playerGems(pid).getCount(gem_color_t(1)),
                        board.playerGems(pid).getCount(gem_color_t(2)),
                        board.playerGems(pid).getCount(gem_color_t(3)),
                        board.playerGems(pid).getCount(gem_color_t(4))
                      });

    const auto ngems = board.playerGems(pid).totalGems();

    // Enumerate all legal moves that take gems of a single color:
    if (ngems <= MAX_PLAYER_GEMS - SAME_COLOR_GEMS) {
        addTakeGemCombination(moves, pid, board, sameColor, table, mine);
    }

    // Enumerate all legal moves that take gems of different colors (no returns):
    if (ngems <= MAX_PLAYER_GEMS - DIFFERENT_COLOR_GEMS) {
        addTakeGemCombination(moves, pid, board, diffColors, table, mine);
    }

    // Enumerate all legal moves that take and return gems (net zero change):
    else if (ngems == MAX_PLAYER_GEMS) {
        addTakeGemCombination(moves, pid, board, netAddZero1, table, mine);
        addTakeGemCombination(moves, pid, board, netAddZero2, table, mine);
    }

    // Enumerate all legal moves that take and return gems (net one gem taken):
    else if (ngems == MAX_PLAYER_GEMS - 1) {
        addTakeGemCombination(moves, pid, board, netAddOne1, table, mine);
        addTakeGemCombination(moves, pid, board, netAddOne2, table, mine);
    }

    // Enumerate all legal moves that take and return gems (net two gems taken):
    else if (ngems == MAX_PLAYER_GEMS - 2) {
        addTakeGemCombination(moves, pid, board, netAddTwo1, table, mine);
        addTakeGemCombination(moves, pid, board, netAddTwo2, table, mine);
    }
}


///////////////////////////////////////////////////////////////////
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


///////////////////////////////////////////////////////////////////
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


///////////////////////////////////////////////////////////////////
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


///////////////////////////////////////////////////////////////////
// Evaluate a move on a throwaway board to check its legality:
MoveStatus
isLegalMove(Board board, player_id_t pid, const GameMove& move, Cards hidden)
{
    return makeMove(board, pid, move, hidden);
}


///////////////////////////////////////////////////////////////////
// Execute a given move on a given board
MoveStatus
makeMove(Board& board, player_id_t pid, const GameMove& mymove,
         Cards& myhidden, const Card& replacement)
{
    MoveStatus status = LEGAL_MOVE;

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

///////////////////////////////////////////////////////////////////
// Chose a move for a given player, find replacement card if necessary,
// and execute the move.
static MoveStatus
playerMove(Board& board, player_id_t pid, Cards& hidden, Cards& deck,
           const Player* player, const Moves& legal)
{
    if (legal.empty()) {
        return LEGAL_MOVE;
    }

    auto pMove = player->getMove(board, hidden, legal);

    // Find replacement card if buying/reserving from table:
    Card replacement = NULL_CARD;
    Card payloadCard = pMove.payload_.card_;


    switch (pMove.type_) {
    case TAKE_GEMS: break;    // No need to replace any cards
    case RESERVE_CARD:
        if (payloadCard.isWild()) {
            payloadCard = popFromDeck(payloadCard.id_.type_, deck);
            assert(!payloadCard.isNull());
        }
        // Fall through to next case:
    case BUY_CARD:
        if (cardIn(pMove.payload_.card_.id_, board.tableCards())) {
            replacement = popFromDeck(payloadCard.id_.type_, deck);
        }
        break;
    }

    const GameMove newMove = (pMove.type_ == TAKE_GEMS)?
                             pMove :
                             GameMove(payloadCard, pMove.type_);

    const auto nobles = board.tableNobles();
    MoveStatus status = makeMove(board, pid, newMove, hidden, replacement);
    assert(status == LEGAL_MOVE);

    MoveNotifier::instance().notifyObservers(
            MoveEvent::MOVE_TAKEN, board, pid, { pMove });
    if (board.tableNobles() != nobles) {
        for (auto n : nobles) {
            if (board.tableNobles().cend() ==
                    find(board.tableNobles().cbegin(), board.tableNobles().cend(), n)) {
                MoveNotifier::instance().notifyObservers(MoveEvent::NOBLE_WON, board, pid, n);
            }
        }
    }

    return status;
}


///////////////////////////////////////////////////////////////////
// TODO: Bypass mode that skips all the error checks in make move, if a move is forced to be legal.
player_id_t
mainGameLoop(Board& board, Cards& deck, Players& players)
{
    Cards hiddenReserves[MAX_NPLAYER];
    MoveNotifier::instance().notifyObservers(MoveEvent::GAME_BEGAN, board, 0);

    while (!board.gameOver()) {
        board.newRound();
        for (player_id_t pid = 0; pid < board.playersNum(); ++pid) {
            const auto legal = legalMoves(board, pid, hiddenReserves[pid]);
            playerMove(board, pid, hiddenReserves[pid], deck, players[pid], legal);
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