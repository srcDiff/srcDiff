/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * Copyright (C) 2024-2024  SDML (www.srcDiff.org)
 * This file is part of the srcDiff translator.
 */
#ifndef INCLUDED_NESTING_BLOCK_HPP
#define INCLUDED_NESTING_BLOCK_HPP

#include "rule_checker.hpp"

namespace nest {

class block : public rule_checker {

public:
    block(const construct& client);

    virtual bool is_nestable_internal(const construct& modified) const;
    
private:
};

}

#endif
