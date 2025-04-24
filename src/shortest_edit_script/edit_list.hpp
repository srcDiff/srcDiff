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

#include <list>
#include <iostream>

namespace ses {

enum edit_operation { COMMON, DELETE, INSERT, CHANGE };

struct edit {
    edit(edit_operation operation,
         std::size_t original_offset = 0, std::size_t original_length = 0,
         std::size_t modified_offset = 0, std::size_t modified_length = 0)
        : operation(operation),
          original_offset(original_offset), original_length(original_length),
          modified_offset(modified_offset), modified_length(modified_length) {
    }

    edit_operation operation;

    std::size_t original_offset;
    std::size_t original_length;

    std::size_t modified_offset;
    std::size_t modified_length;

};

class edit_list : public std::list<edit> {
public:

    void debug() {
        for(const struct edit& edit : *this) {
            std::cerr << "operation:"       << edit.operation << '\n';
            std::cerr << "original_offset:" << edit.original_offset << '\n';
            std::cerr << "original_length:" << edit.original_length << '\n';
            std::cerr << "modified_offset:" << edit.modified_offset << '\n';
            std::cerr << "modified_length:" << edit.modified_length << '\n';
        }
    }

};

typedef edit_list::iterator edit_iterator;

}

#endif