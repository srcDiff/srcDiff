/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * Copyright (C) 2023-2024  SDML (www.srcDiff.org)
 * This file is part of the srcDiff translator.
 */
#ifndef INCLUDED_EXPR_STMT_HPP
#define INCLUDED_EXPR_STMT_HPP

#include <expr_construct.hpp>

class expr_stmt : public expr_construct {

public:

    template<class nest_rule_checker>
    expr_stmt(const construct* parent, std::size_t& start, std::shared_ptr<nest_rule_checker> nest_checker)
        : construct(parent, start, std::make_shared<nest_rule_checker>(*this)), expr_construct(parent, start, std::make_shared<nest_rule_checker>(*this)) {
    }
    virtual bool check_nest(const construct & modified) const;
    virtual bool is_matchable_impl(const construct & modified) const;
};


#endif
