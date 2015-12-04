//
// Created by eitan on 12/4/15.
//

#include "text_player.h"
#include "board.h"

#include <cassert>
#include <iostream>
#include <iomanip>

using namespace std;

namespace grandeur {

TextPlayer::TextPlayer(player_id_t pid)
   : Player(pid), firstTime_(true)
{
    MoveNotifier::instance().registerObserver(
            [=](const Board& board, player_id_t pid, const GameMove& mv) {
                moveUpdater(board, pid, mv);
            });
}


/////////////////////////////////////////////////////////////
GameMove
TextPlayer::getMove(const Board& board, const Cards& hidden, const Moves& legal) const
{
    if (legal.empty()) {
        cout << "You have no legal moves available, skipping your turn...\n";
        return NULL_MOVE;
    }

    cout << "\nYour turn! Board state:\n" << board;
    if (!hidden.empty()) {
        cout << "Hidden reserve cards:\n";
        for (auto& c : hidden) {
            cout << c << "\n";
        }
    }

    cout << "\nList of legal moves available to you:\n";
    int i = 0;
    for (auto m : legal) {
        cout << "[" << setw(2) << ++i << "]\t";
        cout << m << endl;
    }

    cout << "Choose a move # (1-" << legal.size() << ")\n";
    int idx;
    while (!(cin >> idx) || idx < 1 || idx > legal.size()) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Wrong move number, try again: ";
    }
    return legal.at(idx - 1);
}


/////////////////////////////////////////////////////////////
void
TextPlayer::moveUpdater(const Board& board, player_id_t pid, const GameMove& mv)
{
    if (firstTime_) {
        assert(mv == NULL_MOVE);
        cout << "Initial board state:\n" << board << "\n";
        firstTime_ = false;
    }
    else if (mv == NULL_MOVE) {  // Last move
        cout << "GAME OVER! ";
        if (pid == Player::pid_) {
            cout << "you won!";
        } else if (pid < board.playersNum()) {
            cout << "Player " << pid << " wons!";
        } else {
            cout << "Stalemate!";
        }
        cout << endl;
    }

    else if (pid != Player::pid_){
        cout << "Player " << pid << " made move: " << mv << "\n";
    }
}


/////////////////////////////////////////////////////////////
static PlayerFactory::Registrator registrator("text",
                                              [](player_id_t pid){ return new TextPlayer(pid); });



} // namespace