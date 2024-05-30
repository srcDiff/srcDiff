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

    bool is_nestable(const construct& modified) const;
    bool is_same_nestable(const construct& modified) const;

    virtual bool is_nestable_internal(const construct& modified) const;

protected:
    const construct& client;
};

}

#endif
