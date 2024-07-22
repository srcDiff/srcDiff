// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file type_query.cpp
 *
 * @copyright Copyright (C) 2015-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include <type_query.hpp>

bool is_simple_type(const std::string & type_name) {

    return type_name == "throw" || type_name == "try" || type_name == "else" || type_name == "literal" || type_name == "operator"
        || type_name == "modifier" || type_name == "cpp:if" || type_name == "cpp:elif" || type_name == "cpp:else"
        || type_name == "cpp:endif" || type_name == "cpp:ifdef" || type_name == "cpp:ifndef" || type_name == "cpp:pragma" || type_name == "cpp:error"
        || type_name == "cpp:region" || type_name == "cpp:endregion" || type_name == "default";

}

bool is_access_specifier(const std::string & type_name) {
    return type_name == "public" || type_name == "protected" || type_name == "private";
}

bool is_function_type(const std::string & type_name) {

    return type_name == "function" || type_name == "function_decl" || type_name == "constructor" || type_name == "constructor_decl"
        || type_name == "destructor" || type_name == "destructor_decl";

}

bool is_class_type(const std::string & type_name) {

    return type_name == "class" || type_name == "class_decl" || type_name == "struct" || type_name == "struct_decl"
        || type_name == "union" || type_name == "union_decl" || type_name == "enum";

}

bool is_template(const std::string & type_name) {

    return type_name == "template";

}

bool is_condition_type(const std::string & type_name) {

    return has_then_clause(type_name) || type_name == "switch" || type_name == "elseif" || type_name == "while"
        || type_name == "do" || type_name == "for" || type_name == "foreach" || type_name == "forever";

}

bool is_catch(const std::string & type_name) {

    return type_name == "catch";

}

bool is_call(const std::string & type_name) {

    return type_name == "call" || type_name == "macro" || type_name == "cast";

}

bool is_decl_stmt(const std::string & type_name) {

    return type_name == "decl_stmt";

}

bool is_preprocessor_special(const std::string & type_name) {

    return type_name == "cpp:define" || type_name == "cpp:include" || type_name == "cpp:line" || type_name == "cpp:undef" || type_name == "cpp:ifdef" || type_name == "cpp:ifndef";

}

bool is_expr(const std::string & type_name) {

return type_name == "expr";

}

bool is_lambda(const std::string & type_name) {

    return type_name == "lambda";

}

bool is_parameter(const std::string & type_name) {

    return type_name == "parameter";

}

bool is_specifier(const std::string & type_name) {

    return type_name == "specifier";

}

bool is_expr_stmt(const std::string & type_name) {

    return type_name == "expr_stmt";

}

bool is_identifier(const std::string & type_name) {

    return type_name == "name";

}

bool is_argument(const std::string & type_name) {

    return type_name == "argument";

}

bool is_comment(const std::string & type_name) {

    return type_name == "comment";
}

bool is_emit(const std::string & type_name) {

    return type_name == "emit";
}

bool is_jump(const std::string & type_name) {

    return type_name == "return" || type_name == "break" || type_name == "continue" || type_name == "goto";

}

bool is_if(const std::string & type_name) {

    return type_name == "if";

}

bool is_ternary(const std::string & type_name) {

    return type_name == "ternary";

}

bool is_label(const std::string & type_name) {

    return type_name == "label" || type_name == "case";

}

bool is_init(const std::string & type_name) {

    return type_name == "init";

}

bool is_type(const std::string & type_name) {

    return type_name == "type";

}

bool is_block(const std::string & type_name) {

    return type_name == "block";

}

bool is_expr_block(const std::string & type_name) {

    return type_name == "expr_block";

}

bool is_attribute(const std::string & type_name) {
    return type_name == "annotation" || type_name == "attribute";
}

bool is_interchange(const std::string & type_name) {

    return type_name == "interchange";

}

bool has_then_clause(const std::string & type_name) {

    return is_if(type_name) || is_ternary(type_name);

}

bool has_body(const std::string & type_name) {

    return type_name == "unit" || is_function_type(type_name) || is_class_type(type_name)
        || (!is_ternary(type_name) && is_condition_type(type_name)) || type_name == "else" || is_expr_block(type_name);

}

/** @todo what should I consider a statement. */
bool is_statement(const std::string & type_name) {

    return (!is_ternary(type_name) && type_name != "elseif" && is_condition_type(type_name)) || is_decl_stmt(type_name) || is_expr_stmt(type_name) || is_jump(type_name)
        || type_name == "try" || type_name == "catch" || type_name == "case"   || type_name == "default" || type_name == "macro";

}

bool is_exception_handling(const std::string & type_name) {

    return type_name == "try" || is_catch(type_name);

}
