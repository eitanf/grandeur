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
#include "noble.h"
#include "move.h"
#include "config.h"
#include "board.h"


#include <algorithm>
#include <string>
#include <vector>

using namespace std;

namespace grandeur {



/////////////////////////////////////////////////////////////
// Shuffle cards and nobles to create randomized game Board
Board
createBoard(Config& config, Cards& deck)
{
    // Copy initial 12 cards to initial and remove from deck
    Cards initialCards;
    for (int dt = LOW; dt <= HIGH; ++dt) {
            for (int i = 0; i < INITIAL_DECK_NCARD; ++i) {
                initialCards.push_back(popFromDeck(deck_t(dt), deck));
            }
    }

    // Copy, shuffle, and truncate nobles
    vector<Noble> nobles(begin(g_nobles), end(g_nobles));
    shuffle(begin(nobles), end(nobles), config.prng_);
    nobles.erase(nobles.begin() + g_noble_allocation[config.players_.size()], nobles.end());

    return (Board(config.players_.size(), initialCards, nobles));
}



}  // namespace

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
int main(int argc, char** argv)
{
    using namespace grandeur;


    Config config(vector<string>(argv + 1, argv + argc));

    // Create shuffled card deck:
    Cards deck(begin(g_deck), end(g_deck));
    shuffle(begin(deck), end(deck), config.prng_);
    auto board = createBoard(config, deck);

    mainGameLoop(board, deck, config.players_);

    return 0;
}