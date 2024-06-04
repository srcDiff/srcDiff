/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * Copyright (C) 2023-2024  SDML (www.srcDiff.org)
 * This file is part of the srcDiff translator.
 */
#ifndef INCLUDED_NAME_HPP
#define INCLUDED_NAME_HPP

#include <construct.hpp>

class name_t : public construct {

public:

    name_t(const construct* parent, std::size_t& start)
        : construct(parent, start) {
    }

    std::string simple_name() const;
    bool check_nest_name(const construct & modified, bool find_name_parent = true) const;
    virtual bool check_nest(const construct & modified) const;
    virtual bool is_matchable_impl(const construct & modified) const;

};


#endif
