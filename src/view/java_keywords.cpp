// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file java_keywords.cpp
 *
 * @copyright Copyright (C) 2016-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include <java_keywords.hpp>

#include <theme.hpp>

java_keywords::java_keywords(const theme_t & theme) : keywords_t(theme) {

    color_map = {

        { "do",           theme.keyword_color },
        { "else",         theme.keyword_color },
        { "for",          theme.keyword_color },
        { "if",           theme.keyword_color },
        { "switch",       theme.keyword_color },
        { "while",        theme.keyword_color },

        { "case",         theme.keyword_color },
        { "default",      theme.keyword_color },

        { "try",          theme.keyword_color },
        { "catch",        theme.keyword_color },
        { "finally",      theme.keyword_color },
        { "throw",        theme.keyword_color },

        { "break",        theme.keyword_color },
        { "continue",     theme.keyword_color },
        { "goto",         theme.keyword_color },
        { "return",       theme.keyword_color },

        { "synchronized", theme.keyword_color },
        { "abstract",     theme.keyword_color },

        { "package",      theme.keyword_color },
        { "import",       theme.keyword_color },

        { "assert",       theme.keyword_color },
        { "new",          theme.keyword_color },
        { "instanceof",   theme.keyword_color },

        { "static",       theme.storage_color },
        { "native",       theme.storage_color },
        { "strictfp",     theme.storage_color },
        { "transient",    theme.storage_color },
        { "volatile",     theme.storage_color },

        { "private",      theme.keyword_color },
        { "protected",    theme.keyword_color },
        { "public",       theme.keyword_color },

        { "const",        theme.storage_color },
        { "explicit",     theme.storage_color },

        { "boolean",      theme.type_color },
        { "byte",         theme.type_color },
        { "char",         theme.type_color },
        { "class",        theme.type_color },
        { "double",       theme.type_color },
        { "enum",         theme.type_color },
        { "final",        theme.type_color },
        { "float",        theme.type_color },
        { "int",          theme.type_color },
        { "interface",    theme.type_color },
        { "long",         theme.type_color },
        { "short",        theme.type_color },
        { "void",         theme.type_color },

    };

}
