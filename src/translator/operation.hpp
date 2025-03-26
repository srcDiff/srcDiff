// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file operation.hpp
 *
 * @copyright Copyright (C) 2025-2025 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */


#ifndef INCLUDED_OPERATION_HPP
#define INCLUDED_OPERATION_HPP

namespace srcdiff {

enum operation {
    NONE,
    COMMON,
    DELETE,
    INSERT,
    CHANGE,
    NEST,
    MOVE
};

}

#endif