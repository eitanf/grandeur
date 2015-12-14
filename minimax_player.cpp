//
// Created by eitan on 12/8/15.
//

#include "minimax_player.h"

#include <tbb/parallel_for.h>

#include <cassert>
#include <iostream>

using namespace std;

namespace grandeur {


//////////////////////////////////////////////////////////////////////////////////
MinimaxPlayer::MinimaxPlayer(unsigned maxDepth, const evaluator_t& eval, player_id_t pid,
                             score_t agingWeight)
        : Player(pid), depth_(maxDepth), evaluator_(eval), agingWeight_(agingWeight)
{
    assert(maxDepth > 0 && "Minimum depth is one turn");
}


//////////////////////////////////////////////////////////////////////////////////
GameMove
MinimaxPlayer::getMove(const Board& board, const Moves& legal) const
{
    assert(board.playersNum() == 2 && "Minimax only defined for two players");
    const auto bestMove = bestMoveN(Player::pid_, depth_, board, legal).first;
    return legal.at(bestMove);
}

//////////////////////////////////////////////////////////////////////////////////
// The first element of the return value is the index of the best move.
// The second element of the return value is the cumulative score of the best move.
std::pair<unsigned, score_t>
MinimaxPlayer::bestMoveN(player_id_t pid,          // The player making the current move
                         unsigned depth,           // Depth of recursion (how many more turns)
                         const Board& board,       // Current board state
                         const Moves& legal) const // List of current legal movees
{
    vector<Board> newBoards;
    assert(!legal.empty());

    auto scores = depth * agingWeight_ *
            computeScores(evaluator_, legal, pid, board, newBoards);
    if (depth > 1) {
        tbb::parallel_for(0, int(legal.size()), 1, [&](auto idx)
// sequential        for (unsigned idx = 0; idx < legal.size(); ++idx)
        {
            if (pid == 0) {
                newBoards[idx].newRound();
            }

            // Swap out pid for opponent:
            const auto opMoves = legalMoves(newBoards[idx], 1 - pid);
            if (!opMoves.empty()) {
                const auto best = this->bestMoveN(1 - pid, depth - 1, newBoards[idx], opMoves);
                scores[idx] -= best.second;
            }
        }
        );
    }

    const auto idx = distance(scores.cbegin(), max_element(scores.cbegin(), scores.cend()));
    return { idx, scores.at(idx) };
}


//////////////////////////////////////////////////////////////////////////////////
static const auto comboEval =
        combine({ winCondition, countPoints, countPrestige },
                { 100,          2,           1 } );

static PlayerFactory::Registrator reg1("minimax-1",
                  [](player_id_t pid){ return new MinimaxPlayer(1, comboEval, pid); });
static PlayerFactory::Registrator reg2("minimax-2",
                  [](player_id_t pid){ return new MinimaxPlayer(2, comboEval, pid); });
static PlayerFactory::Registrator reg3("minimax-3",
                  [](player_id_t pid){ return new MinimaxPlayer(3, comboEval, pid, 0.01); });
static PlayerFactory::Registrator reg4("minimax-4",
                  [](player_id_t pid){ return new MinimaxPlayer(4, comboEval, pid, 0.01); });
static PlayerFactory::Registrator reg5("minimax-5",
                  [](player_id_t pid){ return new MinimaxPlayer(5, comboEval, pid); });
static PlayerFactory::Registrator reg6("minimax-6",
                  [](player_id_t pid){ return new MinimaxPlayer(6, comboEval, pid); });

static const auto allEval =
        combine({ winCondition, countPoints, countPrestige, countGems, countMoves, monopolizeGems, preferWildcards, countReturns, preferShortGame },
                { 100,          2,           1,             1,         0,          0,              0,               -1,           1 });


static PlayerFactory::Registrator regs3("special-3",
                                       [](player_id_t pid){ return new MinimaxPlayer(3, allEval, pid, 0.01); });

static PlayerFactory::Registrator regs4("special-4",
                                        [](player_id_t pid){ return new MinimaxPlayer(4, allEval, pid, 0.01); });


} // namespace