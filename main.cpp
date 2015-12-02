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
#include "card.h"
#include "noble.h"
#include "move.h"
#include "player.h"
#include "board.h"

#include <algorithm>
#include <iostream>
#include <random>
#include <string>
#include <vector>

using namespace std;

namespace grandeur {

// A PRNG to initialize game state:
static std::mt19937 prng;


/////////////////////////////////////////////////////////////
void
die(const string msg = "") {
    if (msg != "") {
        cerr << "Error: " << msg << endl;
    }
    cerr << "Usage: [options] p1 p2 [p3 [p4]]\n";
    cerr << "-h --help: display this message\n";
    cerr << "-s --seed num: Set PRNG seed for board generation\n";
    cerr << "\nValid player choices are:";
    for (auto name : PlayerFactory::instance().names()) {
        cerr << "  " << name;
    }
    cerr << endl;

    exit(-1);
}


/////////////////////////////////////////////////////////////
void
initConfig(const vector<string>& args, int& nplayer, Players& players)
{
    for (auto i = args.cbegin(); i != args.cend(); ++i) {
        if (*i == "-h" || *i == "--help") die();

        if (*i == "-s" || *i == "--seed") {
            if (++i == args.cend()) die("missing seed value");
            prng.seed(atoll((i->c_str())));
            continue;
        }

        // If we got here, we have an arbitrary string. Try to make Player:
        players.push_back(PlayerFactory::instance().create(*i, nplayer++));
        if (nullptr == players.back())   die ("unrecognized player " + *i);
    }

    if (nplayer < 2) die("must define at least two players");
}


/////////////////////////////////////////////////////////////
Board
createBoard(int nplayer, Cards& deck)
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


/////////////////////////////////////////////////////////////
void
cleanUp(Players& players)
{
    for (auto p : players) {
        delete p;
    }
}


}  // namespace

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
int main(int argc, char** argv)
{
    using namespace grandeur;

    int nplayer;
    Players players;

    initConfig(vector<string>(argv + 1, argv + argc), nplayer, players);

    // Create shuffled card deck:
    Cards deck(begin(g_deck), end(g_deck));
    random_shuffle(begin(deck), end(deck));
    auto board = createBoard(nplayer, deck);

    mainGameLoop(board, deck, players);

    cleanUp(players);
    return 0;
}