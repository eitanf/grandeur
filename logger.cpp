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

    ofstream ofile_;
    bool firstTime_;
};

/////////////////////////////////////////////////////////////
Logger::Logger(const string& fn)
        : pImpl_(new Impl(fn))
{
    if (!pImpl_->ofile_.is_open()) {
        throw std::runtime_error("Can't write to file" + fn);
    }
}


/////////////////////////////////////////////////////////////
Logger::~Logger()
{
    pImpl_->ofile_.close();
}


/////////////////////////////////////////////////////////////
void
grandeur::Logger::log(const Board& board, player_id_t pid, const GameMove& mv)
{
    if (pImpl_->firstTime_) {
        assert(mv == NULL_MOVE);
        pImpl_->ofile_ << "Initial board state:\n" << board << "\n";
        pImpl_->firstTime_ = false;
    }
    else if (mv == NULL_MOVE) {  // Last move
        pImpl_->ofile_ << "GAME OVER! ";
        if (pid < board.playersNum()) {
            pImpl_->ofile_ << "Player " << pid << " wins!\n";
        } else {
            pImpl_->ofile_ << "Stalemate!\n";
        }
    }
    else {
        pImpl_->ofile_ << "Player " << pid << " made move: " << mv << "\n";
        pImpl_->ofile_ << "New board state:\n" << board << "\n";
    }
}


} // namespace
