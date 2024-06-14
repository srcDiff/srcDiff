/*
 * SPDX-License-Identifier: GPL-3.0-only

 * Copyright (C) 2011-2024  SDML (www.srcDiff.org)
 * This file is part of the srcDiff translator.
 */
#include <expr_construct.hpp>

#include <unordered_set>
#include <string>

std::shared_ptr<const expr_t> expr_construct::expr(const expr_construct & that [[maybe_unused]]) const {
    if(expr_child) return *expr_child;

    expr_child = std::static_pointer_cast<const expr_t>(find_child("expr"));
    return *expr_child;
}
