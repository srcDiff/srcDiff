// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file expr_stmt.hpp
 *
 * @copyright Copyright (C) 2024-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_NEST_EXPR_STMT_HPP
#define INCLUDED_NEST_EXPR_STMT_HPP

#include "rule_checker.hpp"

namespace nest {

class expr_stmt : public rule_checker {
public:
    expr_stmt(const construct& client) : rule_checker(client, {"switch"}) {
    }

    virtual bool check_nest(const construct & modified) const;

private:
};

}

#endif
