//
// Created by eitan on 12/2/15.
//

#include "random_player.h"
#include "config.h"

#include <random>

namespace grandeur {

extern Config* g_config;

GameMove
RandomPlayer::getMove(const Board&, const Cards&, const Moves& legal) const
{
    std::uniform_int_distribution<> dist(0, legal.size() - 1);
    return legal.at(dist(g_config->prng_));
}

static PlayerFactory::Registrator registrator("random",
                             [](player_id_t pid){ return new RandomPlayer(pid); });

} // namespace