/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * Copyright (C) 2024-2024  SDML (www.srcDiff.org)
 * This file is part of the srcDiff translator.
 */

#ifndef INCLUDED_STRING_UTILS_HPP
#define INCLUDED_STRING_UTILS_HPP

#include <unordered_set>
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
