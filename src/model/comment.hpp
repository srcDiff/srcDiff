/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * Copyright (C) 2023-2024  SDML (www.srcDiff.org)
 * This file is part of the srcDiff translator.
 */
#ifndef INCLUDED_COMMENT_HPP
#define INCLUDED_COMMENT_HPP

#include <always_matched_construct.hpp>

class comment_t : public always_matched_construct {

public:

    comment_t(const construct* parent, std::size_t& start)
        : always_matched_construct(parent, start) {
    }
    virtual bool check_nest(const construct & modified) const;

private:
};


#endif
