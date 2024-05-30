/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * Copyright (C) 2024-2024  SDML (www.srcDiff.org)
 * This file is part of the srcDiff translator.
 */
#ifndef INCLUDED_NEST_CLASS_HPP
#define INCLUDED_NEST_CLASS_HPP

#include "rule_checker.hpp"

namespace nest {

class class_t : public rule_checker {

public:
    class_t(const construct& client);

    virtual bool can_nest_internal(const construct& modified) const;
    
private:
};

}

#endif
