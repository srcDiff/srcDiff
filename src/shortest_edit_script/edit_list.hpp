// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file edit_list.hpp
 *
 * @copyright Copyright (C) 2025-2025 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_EDIT_LIST_HPP
#define INCLUDED_EDIT_LIST_HPP

#include <edit.hpp>

#include <list>
#include <iostream>

namespace ses {

class edit_list : public std::list<edit> {
public:

    void debug() {
        for(const struct edit& edit : *this) {
            std::cerr << edit << "\n\n";
        }
    }

};

typedef edit_list::iterator edit_iterator;

}

#endif