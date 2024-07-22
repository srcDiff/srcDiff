// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file expr.hpp
 *
 * @copyright Copyright (C) 2024-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_NEST_EXPR_HPP
#define INCLUDED_NEST_EXPR_HPP

#include "rule_checker.hpp"

namespace nest {

class expr_t : public rule_checker {

public:
    expr_t(const construct& client);

    virtual bool check_nest(const construct & modified) const;

private:
};

}

#endif
