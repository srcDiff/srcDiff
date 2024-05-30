/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * Copyright (C) 2024-2024  SDML (www.srcDiff.org)
 * This file is part of the srcDiff translator.
 */
#ifndef INCLUDED_NEST_CALL_HPP
#define INCLUDED_NEST_CALL_HPP

#include "rule_checker.hpp"

namespace nest {

class call : public rule_checker {

public:
    call(const construct& client);

    virtual bool can_nest_internal(const construct& modified) const;
    
private:
};

}

#endif
