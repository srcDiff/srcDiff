// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file expr_construct.hpp
 *
 * @copyright Copyright (C) 2024-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_CONVERT_EXPR_CONSTRUCT_HPP
#define INCLUDED_CONVERT_EXPR_CONSTRUCT_HPP

class construct;

#include "rule_checker.hpp"

namespace convert {

class expr_construct : public rule_checker {

public:

    expr_construct(const construct& client);

    virtual bool is_convertable_impl(const construct & modified) const;

protected:
};

}

#endif
