// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file rule_checker.hpp
 *
 * @copyright Copyright (C) 2024-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_CONVERT_RULE_CHECKER_HPP
#define INCLUDED_CONVERT_RULE_CHECKER_HPP

class construct;

#include <string_utils.hpp>

namespace convert {

class rule_checker {

public:

    rule_checker(const construct& client, const string_set& convertable_constructs = {});

    virtual bool is_tag_convertable(const construct & modified) const;
    bool is_convertable(const construct & modified) const;
    virtual bool is_convertable_impl(const construct & modified) const;

protected:
    const construct& client;
    string_set convertable_constructs;
};

}

#endif
