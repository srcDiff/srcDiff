/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * Copyright (C) 2023-2024  SDML (www.srcDiff.org)
 * This file is part of the srcDiff translator.
 */
#ifndef INCLUDED_ALWAYS_MATCH_HPP
#define INCLUDED_ALWAYS_MATCH_HPP

#include <construct.hpp>

#include <string>

class always_match : public construct {

public:

    always_match(const construct* parent, std::size_t& start)
        : construct(parent, start) {}
    virtual bool is_matchable_impl(const construct & modified) const;

private:
};


#endif
