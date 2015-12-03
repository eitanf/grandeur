//
// Created by eitan on 12/2/15.
//

#include "random_player.h"

#include <cstdlib>
#include <random>

namespace grandeur {

GameMove
RandomPlayer::getMove(Board&, Cards&, const Moves& legal) const
{
    static const char* seedstr = std::getenv("GRANDEUR_PLAYER_SEED");
    static std::mt19937_64 generator(seedstr? std::atoll(seedstr) : 1ULL);

    std::uniform_int_distribution<> dist(0, legal.size() - 1);
    return legal.at(dist(generator));
}

static PlayerFactory::Registrator registrator("random",
                             [](player_id_t pid){ return new RandomPlayer(pid); });

} // namespace