// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file block.hpp
 *
 * @copyright Copyright (C) 2024-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_NEST_BLOCK_HPP
#define INCLUDED_NEST_BLOCK_HPP

#include "rule_checker.hpp"

namespace nest {

class block : public rule_checker {

public:
    block(const construct& client);

    virtual bool can_nest_internal(const construct& modified) const;
    
private:
};

}

#endif
