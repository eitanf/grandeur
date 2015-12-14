//
// Created by eitan on 12/4/15.
//

#include "text_player.h"
#include "board.h"
#include "move_notifier.h"
#include "eval.h"

#include <cassert>
#include <iostream>
#include <iomanip>

using namespace std;

namespace grandeur {

TextPlayer::TextPlayer(player_id_t pid)
   : Player(pid), os_(cout)
{
    MoveNotifier::instance().registerObserver(
            [=](MoveEvent event, const Board& board, player_id_t pid,
                const MoveNotifier::Payload& payload)
            {
                moveUpdater(event, board, pid, payload);
            });
}


/////////////////////////////////////////////////////////////
GameMove
TextPlayer::getMove(const Board& board, const Moves& legal) const
{

    if (legal.empty()) {
        cout << "You have no legal moves available, skipping your turn...\n";
        return NULL_MOVE;
    }

    cout << "\nYour turn! Board state:\n" << board;
    cout << "Your reserved cards: ";
    for (auto const& c : board.playerReserves(Player::pid_)) {
        cout << c << "\n";
    }


    const auto evaluator = combine({ winCondition, countPoints, countPrestige },
                                   { 100,          2,           1 } );
    vector<Board> newBoards;
    const auto scores = computeScores(evaluator, legal, Player::pid_, board, newBoards);

    cout << "\nList of legal moves available to you:\n";
    int i = 0;
    for (auto m : legal) {
        cout << "[" << setw(2) << ++i << "]\t";
        cout << m;
        cout << "\t(score: " << scores[i - 1] << ")";
        cout << endl;
    }

    cout << "Choose a move # (1-" << legal.size() << ")\n";
    unsigned idx;
    while (!(cin >> idx) || idx < 1 || idx > legal.size()) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Wrong move number, try again: ";
    }
    return legal.at(idx - 1);
}


/////////////////////////////////////////////////////////////
void
TextPlayer::moveUpdater(MoveEvent event, const Board& board, player_id_t pid,
                        const MoveNotifier::Payload& payload)
{
    switch(event) {
    case MoveEvent::GAME_BEGAN:
        os_ << "Initial board state:\n" << board << "\n";
        break;

    case MoveEvent::MOVE_TAKEN:
        if (pid != Player::pid_) {
            os_ << "Player " << pid << " made move: " << payload.mv_ << "\n";
        } else {
            os_ << "New board state:\n" << board << "\n";
        }
        break;

    case MoveEvent::NOBLE_WON:
        os_ << "Player " << pid << " won noble: " << payload.noble_ << "\n";
        break;

    case MoveEvent::GAME_WON:
        os_ << "GAME OVER! ";
        if (pid == Player::pid_) {
            os_ << "you won!\n";
        } else {
            os_ << " Player " << pid << " won!\n";
        }
        break;

    case MoveEvent::TIE:
        os_ << "GAME OVER! Stalemate!\n";
        break;
    }
}


/////////////////////////////////////////////////////////////
static PlayerFactory::Registrator registrator("text",
                                              [](player_id_t pid){ return new TextPlayer(pid); });



} // namespace
