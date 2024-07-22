// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file custom.hpp
 *
 * @copyright Copyright (C) 2024-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_CONVERT_CUSTOM_HPP
#define INCLUDED_CONVERT_CUSTOM_HPP

#include "rule_checker.hpp"

#include <string_utils.hpp>

namespace convert {

template<string_literal... converables>
class custom : public rule_checker {
public:
    custom(const construct& client) : rule_checker(client, {converables...}) {
    }

private:
};

}

#endif
