// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file edit.hpp
 *
 * @copyright Copyright (C) 2025-2025 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_EDIT_HPP
#define INCLUDED_EDIT_HPP

#include <edit_operation.hpp>

#include <iostream>

namespace ses {

struct edit {
    edit(edit_operation operation,
         std::size_t original_offset = 0, std::size_t original_length = 0,
         std::size_t modified_offset = 0, std::size_t modified_length = 0)
        : operation(operation),
          original_offset(original_offset), original_length(original_length),
          modified_offset(modified_offset), modified_length(modified_length) {
    }

    friend std::ostream& operator<<(std::ostream& out, struct edit edit) {
        out << "operation:"       << edit.operation << '\n';
        out << "original_offset:" << edit.original_offset << '\n';
        out << "original_length:" << edit.original_length << '\n';
        out << "modified_offset:" << edit.modified_offset << '\n';
        out << "modified_length:" << edit.modified_length << '\n';
        return out;
    }

    edit_operation operation;

    std::size_t original_offset;
    std::size_t original_length;

    std::size_t modified_offset;
    std::size_t modified_length;

};

}

#endif