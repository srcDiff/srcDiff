#include <type_query.hpp>

bool is_simple_type(const std::string & type_name) {

    return type_name == "throw" || type_name == "try" || type_name == "else" || type_name == "literal" || type_name == "operator"
        || type_name == "modifier" || type_name == "cpp:if" || type_name == "cpp:elif" || type_name == "cpp:else"
        || type_name == "cpp:endif" || type_name == "cpp:ifdef" || type_name == "cpp:ifndef" || type_name == "cpp:pragma" || type_name == "cpp:error"
        || type_name == "cpp:region" || type_name == "cpp:endregion" || type_name == "case" || type_name == "default";

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

    return type_name == "if" || type_name == "switch" || type_name == "elseif" || type_name == "while" || type_name == "do" || type_name == "for" || type_name == "foreach";

}

bool is_catch(const std::string & type_name) {

    return type_name == "catch";

}

bool is_call(const std::string & type_name) {

    return type_name == "call" || type_name == "macro";

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
