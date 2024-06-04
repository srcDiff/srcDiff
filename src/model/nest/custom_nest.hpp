/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * Copyright (C) 2024-2024  SDML (www.srcDiff.org)
 * This file is part of the srcDiff translator.
 */
#ifndef INCLUDED_NEST_CUSTOM_NEST_HPP
#define INCLUDED_NEST_CUSTOM_NEST_HPP

#include "rule_checker.hpp"

#include <string_utils.hpp>

namespace nest {

template<string_literal... nestables>
class custom_nest : public rule_checker {
public:
    custom_nest(const construct& client) : rule_checker(client, {nestables...}) {
    }

private:
};

}

#endif
