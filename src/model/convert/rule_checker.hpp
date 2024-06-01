/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * Copyright (C) 2024-2024 SDML (www.srcDiff.org)
 * This file is part of the srcDiff translator.
 */
#ifndef INCLUDED_CONVERT_RULE_CHECKER_HPP
#define INCLUDED_CONVERT_RULE_CHECKER_HPP

class construct;

#include <unordered_set>
#include <string>

namespace convert {

class rule_checker {

public:

    rule_checker(const construct& client);

    virtual bool is_tag_convertable(const construct & modified) const;
    bool is_convertable(const construct & modified) const;
    virtual bool is_convertable_impl(const construct & modified) const;

protected:
    const construct& client;
};

}

#endif
