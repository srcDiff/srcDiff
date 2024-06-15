// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file clause.cpp
 *
 * @copyright Copyright (C) 2023-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include <clause.hpp>

#include <if.hpp>

std::shared_ptr<const construct> clause::block() const {
    if(block_child) return *block_child;

    block_child = find_child("block");
    return *block_child;
}
