// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file python_keywords.cpp
 *
 * @copyright Copyright (C) 2026 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include <python_keywords.hpp>

#include <theme.hpp>

python_keywords::python_keywords(const theme_t & theme) : keywords_t(theme) {

    color_map = {

        { "elif",     theme.keyword_color },
        { "else",     theme.keyword_color },
        { "for",      theme.keyword_color },
        { "if",       theme.keyword_color },
        { "while",    theme.keyword_color },

        { "match",    theme.keyword_color },
        { "case",     theme.keyword_color },

        { "try",      theme.keyword_color },
        { "except",   theme.keyword_color },
        { "finally",  theme.keyword_color },
        { "raise",    theme.keyword_color },
        { "with",     theme.keyword_color },

        { "break",    theme.keyword_color },
        { "continue", theme.keyword_color },
        { "return",   theme.keyword_color },
        { "yield",    theme.keyword_color },
        { "await",    theme.keyword_color },
        { "pass",     theme.keyword_color },

        { "import",   theme.keyword_color },
        { "from",     theme.keyword_color },
        { "as",       theme.keyword_color },

        { "assert",   theme.keyword_color },
        { "del",      theme.keyword_color },

        { "and",      theme.keyword_color },
        { "or",       theme.keyword_color },
        { "not",      theme.keyword_color },
        { "in",       theme.keyword_color },
        { "is",       theme.keyword_color },

        { "global",   theme.storage_color },
        { "nonlocal", theme.storage_color },
        { "async",    theme.storage_color },

        { "class",    theme.type_color },
        { "def",      theme.type_color },
        { "lambda",   theme.type_color },
        { "type",     theme.type_color },

    };

}
