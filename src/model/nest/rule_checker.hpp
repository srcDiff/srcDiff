// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file rule_checker.hpp
 *
 * @copyright Copyright (C) 2024-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_NEST_RULE_CHECKER_HPP
#define INCLUDED_NEST_RULE_CHECKER_HPP

class construct;

#include <string_utils.hpp>

namespace nest {

class rule_checker {

public:

    rule_checker(const construct& client, const string_set& nestable_constructs = {});

    virtual bool check_nest(const construct & modified) const;

    bool can_nest(const construct& modified) const;
    bool can_nest_same(const construct& modified) const;

    virtual bool can_nest_internal(const construct& modified) const;

protected:
    const construct& client;
    string_set nestable_constructs;
};

}

#endif
