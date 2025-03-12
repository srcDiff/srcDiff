// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file expr_stmt.hpp
 *
 * @copyright Copyright (C) 2025-2025 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_NEST_EXPR_STMT_HPP
#define INCLUDED_NEST_EXPR_STMT_HPP

#include "similar.hpp"

namespace nest {

class expr_stmt : public similar {
public:
    expr_stmt(const construct& client) : similar(client, {"switch"}) {
    }

private:
};

}

#endif
