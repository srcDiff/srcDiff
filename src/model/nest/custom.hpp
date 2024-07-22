// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file custom.hpp
 *
 * @copyright Copyright (C) 2024-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_NEST_CUSTOM_HPP
#define INCLUDED_NEST_CUSTOM_HPP

#include "rule_checker.hpp"

#include <string_utils.hpp>

namespace nest {

template<string_literal... nestables>
class custom : public rule_checker {
public:
    custom(const construct& client) : rule_checker(client, {nestables...}) {
    }

private:
};

}

#endif
