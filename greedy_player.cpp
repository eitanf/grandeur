//
// Created by eitan on 12/6/15.
//

#include "greedy_player.h"

#include <algorithm>

namespace grandeur {

GameMove
GreedyPlayer::getMove(const Board& board, const Cards& hidden, const Moves& legal) const
{
    const auto scores = computeScores(evaluator_, legal, Player::pid_, board, hidden);
    const auto idx = distance(scores.cbegin(), max_element(scores.cbegin(), scores.cend()));
    return legal.at(idx);
}

static const auto comboEval =
        combine({ winCondition, countPoints, countPrestige },
                { 100,          2,           1 } );

static PlayerFactory::Registrator reg2("greedy",
                  [](player_id_t pid){ return new GreedyPlayer(comboEval, pid); });



} // namespace