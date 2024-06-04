/*
 * SPDX-License-Identifier: GPL-3.0-only

 * Copyright (C) 2011-2024  SDML (www.srcDiff.org)
 * This file is part of the srcDiff translator.
 */
#include <clause.hpp>

#include <if.hpp>

std::shared_ptr<const construct> clause::block() const {
    if(block_child) return *block_child;

    block_child = find_child("block");
    return *block_child;
}
