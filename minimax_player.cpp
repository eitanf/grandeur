//
// Created by eitan on 12/8/15.
//

#include "minimax_player.h"

#include <cassert>
#include <vector>

using namespace std;

namespace grandeur {


//////////////////////////////////////////////////////////////////////////////////
MinimaxPlayer::MinimaxPlayer(unsigned maxDepth, const evaluator_t& eval, player_id_t pid)
        : depth_(maxDepth), evaluator_(eval), Player(pid)
{
    assert(maxDepth > 0 && "Minimum depth is one turn");
}


//////////////////////////////////////////////////////////////////////////////////
GameMove
MinimaxPlayer::getMove(const Board& board, const Cards& hidden, const Moves& legal) const
{
    assert(board.playersNum() == 2 && "Minimax only defined for two players");
    const auto bestMove = bestMoveN(depth_, board, hidden, legal).first;
    return legal.at(bestMove);
}

//////////////////////////////////////////////////////////////////////////////////
// The first element of the return value is the index of the best move.
// The second element of the return value is the cummulative score of the best move.
std::pair<unsigned, score_t>
MinimaxPlayer::bestMoveN(unsigned depth,
                          const Board& board,
                          const Cards& hidden,
                          const Moves& legal) const
{
    vector<Board> newBoards;
    vector<Moves> newMoves;
    assert(!legal.empty());

    auto scores = computeScores(evaluator_, legal, Player::pid_, board, hidden,
                                &newBoards, &newMoves);

    if (depth > 1) {
        for (unsigned i = 0; i < legal.size(); ++i) {
            if (!newMoves[i].empty()) {
                const auto best = bestMoveN(depth - 1, newBoards[i], hidden, newMoves[i]);
                scores[i] -= best.second;
            }
        }
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
                  [](player_id_t pid){ return new MinimaxPlayer(3, comboEval, pid); });
static PlayerFactory::Registrator reg4("minimax-4",
                  [](player_id_t pid){ return new MinimaxPlayer(4, comboEval, pid); });
static PlayerFactory::Registrator reg5("minimax-5",
                  [](player_id_t pid){ return new MinimaxPlayer(5, comboEval, pid); });
static PlayerFactory::Registrator reg6("minimax-6",
                  [](player_id_t pid){ return new MinimaxPlayer(6, comboEval, pid); });
static PlayerFactory::Registrator reg7("minimax-7",
                  [](player_id_t pid){ return new MinimaxPlayer(7, comboEval, pid); });
static PlayerFactory::Registrator reg8("minimax-8",
                  [](player_id_t pid){ return new MinimaxPlayer(8, comboEval, pid); });
static PlayerFactory::Registrator reg9("minimax-9",
                  [](player_id_t pid){ return new MinimaxPlayer(9, comboEval, pid); });
static PlayerFactory::Registrator reg10("minimax-10",
                  [](player_id_t pid){ return new MinimaxPlayer(10, comboEval, pid); });

} // namespace