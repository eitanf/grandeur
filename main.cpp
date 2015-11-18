/*
 * grandeur is a board game similar to Splendor(TM).
 * This implementation is a testbed for different AI players.
 *
 * This file includes all the code to set up a new game. The actual game loop
 * and mechanics are in game_rules.cpp.
 *
 * (C) 2015 Eitan Frachtenberg. MIT License.
 */

#include "mechanics.h"

void parseCmdline(int argc, char **argv)
{

}


void createPlayers()
{

}

void createBoard()
{

}

int main(int argc, char** argv)
{
    parseCmdline(argc, argv);
    createPlayers();
    createBoard();

    mainGameLoop();

    return 0;
}