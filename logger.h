// Logger is a class to observe game moves and log them to a file
//
// Created by eitan on 12/2/15.
//

#pragma once

#include "board.h"
#include "move.h"
#include "move_notifier.h"

#include <memory>
#include <string>

namespace grandeur {

struct Logger {
  public:
    Logger(const std::string& fn);
    ~Logger();
    void log(MoveEvent, const Board&, player_id_t, const MoveNotifier::Payload&);

  private:
    struct Impl;
    std::unique_ptr<Impl> pImpl_;
};


} // namespace