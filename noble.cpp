// Definitions for noble class
//
// Created by eitan on 11/25/15.
//

#include "noble.h"

#include <iostream>

namespace grandeur {

bool
operator==(const Noble& lhs, const Noble& rhs)
{
    return lhs.cost_ == rhs.cost_;
}


std::ostream&
operator<<(std::ostream& os, const Noble& noble)
{
    os << "cost: " << noble.cost_;
    os << "\tpoints: " << noble.points_;
    return os;
}


} // namespace