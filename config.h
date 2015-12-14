// Config: a class to initialize and manage game configuration.
// Specifically, the parameters managed are those that the user can control
// through the command line.
//
// Created by eitan on 12/2/15.
//


#pragma once

#include "logger.h"
#include "player.h"

#include <fstream>
#include <random>
#include <string>
#include <vector>

namespace grandeur {


class Config {
  public:
    // Initialize all controllable game parameters from command line args:
    Config(const std::vector<std::string>& args);
    ~Config();
    // Exit program with error message:
    void die(const std::string msg = "");

    std::mt19937_64 prng_; // A PRNG to initialize game state:
    Players players_;
    unsigned nthread_ = 0;  // No. of threads to run

  private:
    Logger* loggerPtr_ = nullptr;
};



} // namespace