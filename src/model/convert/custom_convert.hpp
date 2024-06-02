/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * Copyright (C) 2024-2024  SDML (www.srcDiff.org)
 * This file is part of the srcDiff translator.
 */
#ifndef INCLUDED_CONVERT_CUSTOM_CONVERT_HPP
#define INCLUDED_CONVERT_CUSTOM_CONVERT_HPP

#include "rule_checker.hpp"

#include <string_utils.hpp>

namespace convert {

template<string_literal... converables>
class custom_convert : public rule_checker {
public:
    custom_convert(const construct& client) : rule_checker(client, {converables...}) {
    }

private:
};

}

#endif
