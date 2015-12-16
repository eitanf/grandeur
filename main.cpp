/*
 * grandeur is a board game similar to Splendor(TM).
 * This implementation is a testbed for different AI players.
 *
 * This file includes all the code to set up a new game. The actual game loop
 * and mechanics are in game_rules.cpp.
 *
 * (C) 2015 Eitan Frachtenberg. GPLv2 License.
 */

#include "constants.h"
#include "player.h"
#include "card.h"
#include "move.h"
#include "config.h"
#include "board.h"

#include <tbb/task_scheduler_init.h>

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

namespace grandeur {

Config* g_config;



/////////////////////////////////////////////////////////////
void finalUpdate(MoveEvent event, const Board&, player_id_t pid,
                 const MoveNotifier::Payload&)
{
    if (event == MoveEvent::GAME_WON) {
        cout << "GAME OVER! Player " << pid << " wins!\n";
    } else if (event == MoveEvent::TIE) {
        cout << "GAME OVER! Stalemate!\n";
    }
}

}  // namespace

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
int main(int argc, char** argv)
{
    using namespace grandeur;

    g_config = new Config(vector<string>(argv + 1, argv + argc));
    tbb::task_scheduler_init init(g_config->nthread_);

    // Create shuffled card deck:
    Cards deck(begin(g_deck), end(g_deck));
    shuffle(begin(deck), end(deck), g_config->prng_);
    auto board = g_config->createBoard(deck);
    MoveNotifier::instance().registerObserver(finalUpdate);

    mainGameLoop(board, deck, g_config->players_);

    delete g_config;
    return 0;
}