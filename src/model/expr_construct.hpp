/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * Copyright (C) 2023-2024  SDML (www.srcDiff.org)
 * This file is part of the srcDiff translator.
 */
#ifndef INCLUDED_EXPR_CONSTRUCT_HPP
#define INCLUDED_EXPR_CONSTRUCT_HPP

#include <construct.hpp>

#include <expr.hpp>

class expr_construct : virtual public construct {

public:

    template<class nest_rule_checker>
    expr_construct(const construct* parent, std::size_t& start, std::shared_ptr<nest_rule_checker> nest_checker)
        : construct(parent, start, std::make_shared<nest_rule_checker>(*this)), expr_child() {
    } 

    virtual std::shared_ptr<const expr_t> expr(const expr_construct & that) const;

    virtual bool is_tag_convertable(const construct & modified) const;
    virtual bool is_convertable_impl(const construct & modified) const;
protected:
    mutable std::optional<std::shared_ptr<const expr_t>> expr_child;
};


#endif
