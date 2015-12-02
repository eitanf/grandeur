/*
 * grandeur is a board game similar to Splendor(TM).
 * This implementation is a testbed for different AI players.
 *
 * This file includes all the code to set up a new game. The actual game loop
 * and mechanics are in game_rules.cpp.
 *
 * (C) 2015 Eitan Frachtenberg. GPLv2 License.
 */

#include "board.h"
#include "card.h"
#include "constants.h"
#include "move.h"
#include "noble.h"
#include "player.h"

#include <algorithm>
#include <vector>

using namespace std;

namespace grandeur {

void initConfig(int argc, char **argv, int& nplayer, Players& players)
{
// Create players from factory by string name
}


Board createBoard(int nplayer, Cards& deck)
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
    random_shuffle(begin(nobles), end(nobles));
    nobles.erase(nobles.begin() + g_noble_allocation[nplayer], nobles.end());

    return (Board(nplayer, initialCards, nobles));
}


}  // namspace

int main(int argc, char** argv)
{
    using namespace grandeur;

    int nplayer;
    Players players;

    initConfig(argc, argv, nplayer, players);

    // Create shuffled card deck:
    Cards deck(begin(g_deck), end(g_deck));
    random_shuffle(begin(deck), end(deck));
    auto board = createBoard(nplayer, deck);

    mainGameLoop(board, deck, players);

    return 0;
}