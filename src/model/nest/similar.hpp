// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file similar.hpp
 *
 * @copyright Copyright (C) 2024-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_NEST_SIMILAR_HPP
#define INCLUDED_NEST_SIMILAR_HPP

#include "rule_checker.hpp"

namespace nest {

class similar : public rule_checker {
public:
    similar(const construct& client, const string_set& nestable_constructs = {}) : rule_checker(client, nestable_constructs) {
    }

    virtual bool check_nest(const construct & modified) const;

private:
};

}

#endif
