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

namespace ses {

enum edit_operation { COMMON, DELETE, INSERT, CHANGE };

struct edit {
    edit(edit_operation operation,
         std::size_t original_offset, std::size_t original_length,
         std::size_t modified_offset, std::size_t modified_length)
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
};

}

#endif