// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file condition.cpp
 *
 * @copyright Copyright (C) 2023-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include <condition.hpp>

std::string condition::to_string(bool skip_whitespacee) const {
    std::string str = construct::to_string(skip_whitespacee);

    if(!str.empty() && str.front() == '(') {
        str.erase(str.begin());
    }

    if(!str.empty() && str.back() == ')') {
        str.pop_back();
    }

    return str;
}
