// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file decl_stmt.hpp
 *
 * @copyright Copyright (C) 2023-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_DECL_STMT_HPP
#define INCLUDED_DECL_STMT_HPP

#include <identifier_decl.hpp>
#include <expr_construct.hpp>

class decl_stmt : public identifier_decl, public expr_construct {

public:

    decl_stmt(const construct* parent, std::size_t& start)
        : construct(parent, start), identifier_decl(parent, start), expr_construct(parent, start) {
    }

    virtual std::shared_ptr<const expr_t> expr(const expr_construct & that) const;
};


#endif
