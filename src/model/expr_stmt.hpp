// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file expr_stmt.hpp
 *
 * @copyright Copyright (C) 2023-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_EXPR_STMT_HPP
#define INCLUDED_EXPR_STMT_HPP

#include <expr_construct.hpp>

class expr_stmt : public expr_construct {

public:

    expr_stmt(const construct* parent, std::size_t& start)
        : construct(parent, start), expr_construct(parent, start) {
    }

    virtual bool is_matchable_impl(const construct & modified) const;
};


#endif
