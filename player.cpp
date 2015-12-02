//
// Created by eitan on 12/2/15.
//

#include "player.h"

#include <map>

using namespace std;

namespace grandeur {


struct PlayerFactory::Impl {
    map<name_t, creator_t> map_;
};

PlayerFactory::PlayerFactory()
   : pImpl(new Impl)
{}

PlayerFactory::~PlayerFactory()
{}


void
PlayerFactory::registerPlayer(name_t name, creator_t creator)
{
    pImpl->map_[name] = creator;
}


const Player*
PlayerFactory::create(name_t name, player_id_t pid)
{
    auto iter = pImpl->map_.find(name);
    if (iter == pImpl->map_.end()) {
        return nullptr;
    } else {
        return iter->second(pid);
    }
}


vector<PlayerFactory::name_t>
PlayerFactory::names() const
{
    vector<name_t> ret;
    for (const auto& i : pImpl->map_) {
        ret.push_back(i.first);
    }
    return ret;
}


} // namespace