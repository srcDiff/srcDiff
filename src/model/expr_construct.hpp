// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file expr_construct.hpp
 *
 * @copyright Copyright (C) 2023-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_EXPR_CONSTRUCT_HPP
#define INCLUDED_EXPR_CONSTRUCT_HPP

#include <construct.hpp>

#include <expr.hpp>

class expr_construct : virtual public construct {

public:

    expr_construct(const construct* parent, std::size_t& start)
        : construct(parent, start), expr_child() {
    } 

    virtual std::shared_ptr<const expr_t> expr(const expr_construct & that) const;

protected:
    mutable std::optional<std::shared_ptr<const expr_t>> expr_child;
};


#endif
