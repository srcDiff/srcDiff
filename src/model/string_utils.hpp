// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file string_utils.hpp
 *
 * @copyright Copyright (C) 2024-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_STRING_UTILS_HPP
#define INCLUDED_STRING_UTILS_HPP

#include <unordered_set>
#include<algorithm>
#include <string>

typedef std::unordered_set<std::string> string_set;

template<std::size_t SIZE>
struct string_literal {
    constexpr string_literal(const char (&s)[SIZE]) {
        std::copy_n(s, SIZE, this->str);
    }

    operator std::string() const {
        return str;
    }

    char str[SIZE];
};

#endif
