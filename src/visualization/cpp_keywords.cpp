/**
 *  @file cpp_keywords.hpp
 *
 *  Specifies cpp_keywords color scheme for syntax highlighting.
 *
 *  @author Michael John Decker <mdecker6@kent.edu>
 */

#include <cpp_keywords.hpp>
#include <monokai.hpp>

cpp_keywords::cpp_keywords(const monokai & theme) {

    color_map = {

        { "do",           theme.keyword_color_html },
        { "else",         theme.keyword_color_html },
        { "for",          theme.keyword_color_html },
        { "if",           theme.keyword_color_html },
        { "switch",       theme.keyword_color_html },
        { "while",        theme.keyword_color_html },

        { "case",         theme.keyword_color_html },
        { "default",      theme.keyword_color_html },

        { "try",          theme.keyword_color_html },
        { "catch",        theme.keyword_color_html },
        { "throw",        theme.keyword_color_html },

        { "break",        theme.keyword_color_html },
        { "continue",     theme.keyword_color_html },
        { "goto",         theme.keyword_color_html },
        { "return",       theme.keyword_color_html },

        { "and",          theme.keyword_color_html },
        { "and_eq",       theme.keyword_color_html },
        { "bitand",       theme.keyword_color_html },
        { "bitor",        theme.keyword_color_html },
        { "compl",        theme.keyword_color_html },
        { "not",          theme.keyword_color_html },
        { "not_eq",       theme.keyword_color_html },
        { "or",           theme.keyword_color_html },
        { "or_eq",        theme.keyword_color_html },
        { "xor",          theme.keyword_color_html },
        { "xor_eq",       theme.keyword_color_html },
        { "new",          theme.keyword_color_html },

        { "const",        theme.storage_color_html },
        { "constexpr",    theme.storage_color_html },
        { "extern",       theme.storage_color_html },
        { "inline",       theme.storage_color_html },
        { "mutable",      theme.storage_color_html },
        { "register",     theme.storage_color_html },
        { "static",       theme.storage_color_html },
        { "thread_local", theme.storage_color_html },
        { "virtual",      theme.storage_color_html },
        { "volatile",     theme.storage_color_html },


        { "typename",     theme.storage_color_html },

        { "auto",         theme.type_color_html },
        { "bool",         theme.type_color_html },
        { "char",         theme.type_color_html },
        { "char16_t",     theme.type_color_html },
        { "char32_t",     theme.type_color_html },
        { "class",        theme.type_color_html },
        { "double",       theme.type_color_html },
        { "enum",         theme.type_color_html },
        { "false",        theme.type_color_html },
        { "float",        theme.type_color_html },
        { "int",          theme.type_color_html },
        { "long",         theme.type_color_html },
        { "short",        theme.type_color_html },
        { "signed",       theme.type_color_html },
        { "struct",       theme.type_color_html },
        { "true",         theme.type_color_html },
        { "union",        theme.type_color_html },
        { "unsigned",     theme.type_color_html },
        { "void",         theme.type_color_html },
        { "wchar_t",      theme.type_color_html }

    };

}

std::string cpp_keywords::color(const std::string & token) const {

    try {

        return color_map.at(token);

    } catch(const std::out_of_range & e) {

        return "";

    }

}

