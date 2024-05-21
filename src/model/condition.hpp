/*
 * SPDX-License-Identifier: GPL-3.0-only

 * Copyright (C) 2011-2024  SDML (www.srcDiff.org)
 * This file is part of the srcDiff translator.
 */
#ifndef INCLUDED_CONDITION_HPP
#define INCLUDED_CONDITION_HPP

#include <always_matched_construct.hpp>

class condition : public always_matched_construct {

public:

    condition(const construct* parent, std::size_t& start) : always_matched_construct(parent, start) {}
    virtual std::string to_string(bool skip_whitespace = false) const;

protected:
};


#endif
