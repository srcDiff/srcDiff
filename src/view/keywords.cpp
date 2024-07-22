// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file keywords.cpp
 *
 * @copyright Copyright (C) 2016-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include <keywords.hpp>

#include <theme.hpp>

keywords_t::keywords_t(const theme_t & theme) {

    color_map = {

        { "do",           theme.keyword_color },
        { "else",         theme.keyword_color },
        { "for",          theme.keyword_color },
        { "foreach",      theme.keyword_color },
        { "if",           theme.keyword_color },
        { "switch",       theme.keyword_color },
        { "while",        theme.keyword_color },

        { "case",         theme.keyword_color },
        { "default",      theme.keyword_color },

        { "try",          theme.keyword_color },
        { "catch",        theme.keyword_color },
        { "throw",        theme.keyword_color },

        { "break",        theme.keyword_color },
        { "continue",     theme.keyword_color },
        { "goto",         theme.keyword_color },
        { "return",       theme.keyword_color },

        { "namespace",    theme.keyword_color },
        { "typename",     theme.storage_color },
        { "using",        theme.storage_color },

        { "and",          theme.keyword_color },
        { "and_eq",       theme.keyword_color },
        { "bitand",       theme.keyword_color },
        { "bitor",        theme.keyword_color },
        { "compl",        theme.keyword_color },
        { "not",          theme.keyword_color },
        { "not_eq",       theme.keyword_color },
        { "or",           theme.keyword_color },
        { "or_eq",        theme.keyword_color },
        { "xor",          theme.keyword_color },
        { "xor_eq",       theme.keyword_color },
        { "new",          theme.keyword_color },

        { "const",        theme.storage_color },
        { "constexpr",    theme.storage_color },
        { "extern",       theme.storage_color },
        { "inline",       theme.storage_color },
        { "mutable",      theme.storage_color },
        { "register",     theme.storage_color },
        { "static",       theme.storage_color },
        { "thread_local", theme.storage_color },
        { "virtual",      theme.storage_color },
        { "volatile",     theme.storage_color },

        { "private",      theme.keyword_color },
        { "protected",    theme.keyword_color },
        { "public",       theme.keyword_color },

        { "explicit",     theme.storage_color },
        { "export",       theme.storage_color },

        { "auto",         theme.type_color },
        { "bool",         theme.type_color },
        { "char",         theme.type_color },
        { "char16_t",     theme.type_color },
        { "char32_t",     theme.type_color },
        { "class",        theme.type_color },
        { "double",       theme.type_color },
        { "enum",         theme.type_color },
        { "false",        theme.type_color },
        { "float",        theme.type_color },
        { "int",          theme.type_color },
        { "long",         theme.type_color },
        { "short",        theme.type_color },
        { "signed",       theme.type_color },
        { "struct",       theme.type_color },
        { "template",     theme.type_color },
        { "true",         theme.type_color },
        { "typedef",      theme.type_color },
        { "union",        theme.type_color },
        { "unsigned",     theme.type_color },
        { "void",         theme.type_color },
        { "wchar_t",      theme.type_color }

    };

}

std::string keywords_t::color(const std::string & token) const {
    return color_map.at(token);
}
