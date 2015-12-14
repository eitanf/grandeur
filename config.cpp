//
// Created by eitan on 12/2/15.
//

#include "config.h"
#include "move.h"
#include "move_notifier.h"

#include <iostream>
#include <tbb/task_scheduler_init.h>

namespace grandeur {

using namespace std;

/////////////////////////////////////////////////////////////
Config::Config(const std::vector<std::string>& args)
{
    for (auto i = args.cbegin(); i != args.cend(); ++i) {
        if (*i == "-h" || *i == "--help") die();

        if (*i == "-s" || *i == "--seed") {
            if (++i == args.cend()) die("missing seed value");
            prng_.seed(atoll((i->c_str())));
            continue;
        }

        if (*i == "-t" || *i == "--threads") {
            if (++i == args.cend()) die("missing thread number");
            nthread_ = atoi((i->c_str()));
            continue;
        }

        if (*i == "-l" || *i == "--log") {
            if (++i == args.cend()) die("missing filename");
            loggerPtr_ = new Logger(*i);
            MoveNotifier::instance().registerObserver(
                    [=](MoveEvent event, const Board& board, player_id_t pid, const MoveNotifier::Payload& payload)
                        { loggerPtr_->log(event, board, pid, payload); });
            continue;
        }

        // If we got here, we have an arbitrary string. Try to make Player:
        players_.push_back(PlayerFactory::instance().create(*i, players_.size()));
        if (nullptr == players_.back())   die ("unrecognized player " + *i);
    }

    if (!nthread_)  nthread_ = tbb::task_scheduler_init::default_num_threads();
    if (players_.size() < 2) die("must define at least two players");
}


/////////////////////////////////////////////////////////////
Config::~Config()
{
    for (auto p : players_) {
        delete p;
    }

    if (loggerPtr_) {
        delete loggerPtr_;
    }
}


/////////////////////////////////////////////////////////////
void
Config::die(const string msg) {
    if (msg != "") {
        cerr << "Error: " << msg << endl;
    }
    cerr << "Usage: [options] p1 p2 [p3 [p4]]\n";
    cerr << "-h --help: display this message\n";
    cerr << "-s --seed num: Set PRNG seed for board generation\n";
    cerr << "-t --threads num: No. of threads to use (default: hardware threads)\n";
    cerr << "-l --log filename: Log board and moves to a file\n";
    cerr << "\nValid player choices are:";
    for (auto name : PlayerFactory::instance().names()) {
        cerr << "  " << name;
    }
    cerr << endl;

    exit(-1);
}


}  // namespace