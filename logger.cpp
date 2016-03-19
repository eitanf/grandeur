//
// Created by eitan on 12/2/15.
//

#include "logger.h"

#include <cassert>
#include <fstream>

using namespace std;

namespace grandeur {

struct Logger::Impl {
  public:
    Impl(const string& fn) : ofile_(fn), firstTime_(true) {}
    ~Impl() { ofile_.close(); }

    ofstream ofile_;
    bool firstTime_;
};

/////////////////////////////////////////////////////////////
Logger::Logger(const string& fn)
        : pImpl_(new Impl(fn), [](Impl* impl){ delete impl; })
{
    if (!pImpl_->ofile_.is_open()) {
        throw std::runtime_error("Can't write to file" + fn);
    }
}


/////////////////////////////////////////////////////////////
void
Logger::log(MoveEvent event, const Board& board, player_id_t pid,
            const MoveNotifier::Payload& payload)
{
    switch(event) {
    case MoveEvent::GAME_BEGAN:
        pImpl_->ofile_ << "Initial board state:\n" << board << "\n";
        break;

    case MoveEvent::MOVE_TAKEN:
        pImpl_->ofile_ << "Player " << pid << " made move: " << payload.mv_ << "\n";
        pImpl_->ofile_ << "New board state for round " << board.roundNumber()
                       << ":\n" << board << endl;
        break;

    case MoveEvent::NOBLE_WON:
        pImpl_->ofile_ << "Player " << pid << " won noble: " << payload.noble_ << "!\n\n";
        break;

    case MoveEvent::REPLACEMENT_CARD:
        pImpl_->ofile_ << "Card " << payload.replacement_.id_ << " drawn from deck\n";
        break;

    case MoveEvent::GAME_WON:
        pImpl_->ofile_ << "GAME OVER! Player " << pid << " wins!\n";
        break;

    case MoveEvent::TIE:
        pImpl_->ofile_ << "GAME OVER! Stalemate!\n";
        break;
    }
}


} // namespace
