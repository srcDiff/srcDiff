/*
 * SPDX-License-Identifier: GPL-3.0-only

 * Copyright (C) 2011-2024  SDML (www.srcDiff.org)
 * This file is part of the srcDiff translator.
 */
#ifndef INCLUDED_EXPR_HPP
#define INCLUDED_EXPR_HPP

#include <construct.hpp>

class expr_t : public construct {

public:

    expr_t(const construct* parent, std::size_t& start) : construct(parent, start) {}
    bool is_single_call() const;
    virtual bool can_nest(const construct & modified) const;
    virtual bool is_matchable_impl(const construct & modified) const;
};


#endif
