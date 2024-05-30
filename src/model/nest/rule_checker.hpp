/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * Copyright (C) 2024-2024 SDML (www.srcDiff.org)
 * This file is part of the srcDiff translator.
 */
#ifndef INCLUDED_NEST_RULE_CHECKER_HPP
#define INCLUDED_NEST_RULE_CHECKER_HPP

class construct;
namespace nest {

class rule_checker {

public:

    rule_checker(const construct& client);

    bool can_nest(const construct& modified) const;
    bool can_nest_same(const construct& modified) const;

    virtual bool can_nest_internal(const construct& modified) const;

protected:
    const construct& client;
};

}

#endif
